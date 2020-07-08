#include "Console.h"
#include <cstring>
#include <cstdio>

void Console::WritePromptAndCommand(const char *command) {
	//TODO make safe
	char promptAndCmd[256];
	sprintf(promptAndCmd, "> %s\n", command);
	Write(promptAndCmd);
}

	void Console::WriteInvalidCommand() {
		WriteLine("Command is not valid");
	}

	void Console::Write(const char *text)
	{
		int len = strlen(text);
		int newHistoryWriteIndex = (m_HistoryWriteIndex + len) % MAX_HISTORY_CHARACTERS_NUM;

		if ( (newHistoryWriteIndex > m_HistoryWriteIndex) || (newHistoryWriteIndex == 0) ) {
			std::strcpy(&m_History[m_HistoryWriteIndex], text);
			m_HistoryWriteIndex = newHistoryWriteIndex;
		} else {
			int firstPartTextLen = MAX_HISTORY_CHARACTERS_NUM - m_HistoryWriteIndex;
			int secondPartTextLen = len - firstPartTextLen;
			std::memcpy(&m_History[m_HistoryWriteIndex], text, firstPartTextLen);
			std::memcpy(&m_History[0], text+firstPartTextLen, secondPartTextLen);
			m_HistoryWriteIndex = newHistoryWriteIndex;
		}

		m_HistoryChanged = true;
	}

	void Console::WriteLine(const char *text) {
		int len = std::strlen(text);
		char textWithNewLine[len+1];
		std::sprintf(textWithNewLine, "%s\n", text);
		Write(textWithNewLine);
	}

	void Console::WriteLine(const std::string &text) {
		WriteLine(text.c_str());
	}

