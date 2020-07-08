#ifndef COMMANDMANAGER_H_
#define COMMANDMANAGER_H_

#include <cstring>

class CommandManager {

public:

	enum {
		CMD_HELLOWORLD,
		CMD_HELP,
		CMD_SAY,
		CMD_SHOW_CHAT,
		CMD_MONITOR,
		NUM_COMMANDS
	};

	struct ArgumentDef {
		enum  {
			TYPE_INT,
			TYPE_FLOAT,
			TYPE_STRING
		};
		static const int MAX_ARGUMENT_SIZE = 32;
	};

	struct CommandDef {
		static const int MAX_NAME_SIZE = 32;
		static const int MAX_DESC_SIZE = 128;
		static const int MAX_SYNTAX_SIZE = 128;
		static const int MAX_ARGUMENTS_NUM = 16;
		int id;
		char name[CommandDef::MAX_NAME_SIZE];
		char desc[CommandDef::MAX_DESC_SIZE];
		char syntax[CommandDef::MAX_SYNTAX_SIZE];
		int expectedArgsNum;
		int argTypes[MAX_ARGUMENTS_NUM];
	};

	struct Argument {
		char value[ArgumentDef::MAX_ARGUMENT_SIZE];
	};

	struct Command {
		int id;
		char name[CommandDef::MAX_NAME_SIZE];
		int type; // client (ex :modify variable of client), server (send chat message)
		int argsNum;
		Argument args[CommandDef::MAX_ARGUMENTS_NUM];

		void Reset() {
			std::strcpy(name, "nocommand");
			type = 0;
			argsNum = 0;
			std::memset(args, 0, sizeof(Argument) * CommandDef::MAX_ARGUMENTS_NUM);
		}
	};

	static CommandManager & Instance() {
		static CommandManager instance;
		return instance;
	}

	void Init();
	void CreateCommand(int id, const char *name, const char *desc, const char *syntax = "", int expectedArgsNum = 0, ...);
	bool AddCommand(const char *command);
	Command * GetNextCommand();
	int GetAllCommandDescSize() const;
	void GetAllCommandDesc(char *allDesc);

private:
	static const int MAX_COMMANDS_TO_TREAT_NUM = 10;
	static const int MAX_COMMAND_SIZE = CommandDef::MAX_NAME_SIZE + CommandDef::MAX_ARGUMENTS_NUM * ArgumentDef::MAX_ARGUMENT_SIZE;
	static const int MAX_COMMANDS_NUM = 128;
	static const int DESC_SIZE = CommandDef::MAX_NAME_SIZE + CommandDef::MAX_DESC_SIZE + 4;

	enum {
		CLIENT_COMMAND,
		SERVER_COMMAND
	};

	CommandManager() : m_CurrentCommandsNum(0), m_CurCmdIndex(0) {Init();}
	CommandManager(const CommandManager &);
	CommandManager operator=(const CommandManager &);

	const char * GetCommandName(int cmdId);
	CommandManager::CommandDef * FindCommandDef(const char *cmdName);
	int GetArgumentType(const char *arg) const;

	CommandDef m_CommandDefs[MAX_COMMANDS_NUM];
	Command m_CommandsToTreat[MAX_COMMANDS_TO_TREAT_NUM];
	int m_CurrentCommandsNum;
	int m_CurCmdIndex;

public:
	static const int ALL_DESC_SIZE = NUM_COMMANDS * DESC_SIZE + 1;

};


//static const char *cmdRegExp =
//		"["
//		CMD_HELLOWORLD
//		" "
//		CMD_SAY
//		"]"
//		"\ [alphanumeric]";

#endif /* COMMANDMANAGER_H_ */
