#include "CommandManager.h"
#include <cstdio>
#include <cassert>
#include <cstdarg>
#include <base/utils.h>
#include <engine/client/Console.h>


void CommandManager::Init() {
	CreateCommand(CMD_HELLOWORLD, "helloworld", "Need a description ? a coffee ? a mars ? 100 dollars ?");
	CreateCommand(CMD_HELP, "help", "Show the descriptions of commands");
	CreateCommand(CMD_SAY, "say", "Say something in the chat", "say [message]");
	CreateCommand(CMD_SHOW_CHAT, "show_chat", "Show the chat window");
	CreateCommand(CMD_MONITOR, "monitor", "enable/disable the monitor", "monitor [0 or 1]", 1, ArgumentDef::TYPE_INT);
}

void CommandManager::CreateCommand(int id, const char *name, const char *desc, const char *syntax, int expectedArgsNum, ...) {

	m_CommandDefs[id].id = id;
	std::strcpy(m_CommandDefs[id].name, name);
	std::strcpy(m_CommandDefs[id].desc, desc);
	std::strcpy(m_CommandDefs[id].syntax, syntax);

	m_CommandDefs[id].expectedArgsNum = expectedArgsNum;
	if (expectedArgsNum > 0) {
		va_list vl;
		va_start(vl, expectedArgsNum);
		for (int i = 0; i < expectedArgsNum; ++i) {
			m_CommandDefs[id].argTypes[i] = va_arg(vl,int);
		}
		va_end(vl);
	}
}

CommandManager::CommandDef * CommandManager::FindCommandDef(const char *cmdName) {
	for (int i = 0; i < MAX_COMMANDS_NUM; ++i)
		if (IsEqualString(cmdName, m_CommandDefs[i].name))
			return &m_CommandDefs[i];
	return 0;
}

int CommandManager::GetArgumentType(const char *arg) const {
	return ArgumentDef::TYPE_INT;
}


bool CommandManager::AddCommand(const char *command) //TODO break in some functions
{
	assert(command);

	Console::Instance().WritePromptAndCommand(command);

	if (std::strlen(command) == 0) {
		Console::Instance().WriteLine("the command is invalid");
		return false;
	}

	if (m_CurrentCommandsNum >= MAX_COMMANDS_TO_TREAT_NUM) {
		Console::Instance().WriteLine("Number of commands max per loop is reached");
		return false;
	}

	char cmdStr[MAX_COMMAND_SIZE];
	std::strcpy(cmdStr, command);


	Command &cmdToAdd = m_CommandsToTreat[m_CurrentCommandsNum];
	cmdToAdd.Reset();

	// --- PARSE NAME ---

	char delimiter[2] = {' ','\0'};
	char *token = std::strtok(cmdStr, delimiter); //FIXME check no token first time

	if (strlen(token) > CommandDef::MAX_NAME_SIZE-1) {
		Console::Instance().WriteLine("Command name is too big");
		return false;
	}

	std::strcpy(cmdToAdd.name, token);

	CommandDef *cmdDef = FindCommandDef(cmdToAdd.name);

	if (!cmdDef) {
		Console::Instance().WriteLine("The command is unknown. Enter 'help' to see possible commands.");
		return false;
	}

	cmdToAdd.id = cmdDef->id;

	// --- PARSE ARGUMENTS ---

	// don't break a chat sentence in arguments
	if (cmdToAdd.id == CMD_SAY)
	{
		delimiter[0] = '\0';
		token = strtok(0, delimiter);
		std::strcpy(cmdToAdd.args[0].value, token);
	}
	else
	{
		int argIndex = 0;
		while (1)
		{
			if (cmdToAdd.argsNum > cmdDef->expectedArgsNum) {
				Console::Instance().WriteLine("Number of arguments is invalid"); //TODO gives number of args
				return false;
			}

			token = strtok(0, delimiter);

			if (token == 0)
				break;

			if (strlen(token) > ArgumentDef::MAX_ARGUMENT_SIZE-1) {
				Console::Instance().WriteLine("Size of one argument is too big");
				return false;
			}

			int argType = GetArgumentType(token);
			if (argType != cmdDef->argTypes[argIndex]) {
				Console::Instance().WriteLine("Type of one argument is invalid");
				return false;
			}

			std::strcpy(cmdToAdd.args[argIndex++].value, token);
			cmdToAdd.argsNum++;
		}

		if (cmdToAdd.argsNum != cmdDef->expectedArgsNum) {
			Console::Instance().WriteLine("Number of arguments is invalid"); //TODO gives number of args
			return false;
		}
	}

	++m_CurrentCommandsNum;

	return true;
}

CommandManager::Command * CommandManager::GetNextCommand()
{
	if (m_CurCmdIndex >= m_CurrentCommandsNum) {
		m_CurCmdIndex = 0;
		m_CurrentCommandsNum = 0;
		return 0;
	}

	return &m_CommandsToTreat[m_CurCmdIndex++];
}

const char * CommandManager::GetCommandName(int cmdId) {
	assert(cmdId >= 0 && cmdId < NUM_COMMANDS);
	return m_CommandDefs[cmdId].name;
}

void CommandManager::GetAllCommandDesc(char *allDesc) {
	char desc[DESC_SIZE];
	allDesc[0] = '\0';
	for (int i = 0; i < NUM_COMMANDS; ++i) {
		std::sprintf(desc, "%s : %s\n", m_CommandDefs[i].name, m_CommandDefs[i].desc);
		std::strcat(allDesc, desc);
	}
}
