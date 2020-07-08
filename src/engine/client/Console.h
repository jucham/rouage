#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <string>

class Console {

public:
	static Console & Instance() {
		static Console instance;
		return instance;
	}

	bool IsActive() const {return m_Active;}
	void SetActive(bool b) {m_Active = b;}
	void WritePromptAndCommand(const char *command);
	void WriteInvalidCommand();

	void Write(const char *text);
	void WriteLine(const char *text);
	void WriteLine(const std::string &text);
	const char * GetHistory() const {return m_History;}
	bool HistoryChanged() const {return m_HistoryChanged;}
	void SetHistoryAsUnchanged() {m_HistoryChanged = false;}

private:
	static const int MAX_HISTORY_CHARACTERS_NUM = 8192;

	Console() : m_Active(false), m_HistoryWriteIndex(0) {}
	Console(const Console &);
	Console operator=(const Console &);

	bool m_Active;
	bool m_HistoryChanged;
	char m_History[MAX_HISTORY_CHARACTERS_NUM];
	int m_HistoryWriteIndex;
};

#endif /* CONSOLE_H_ */
