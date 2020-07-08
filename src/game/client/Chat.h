#ifndef TEXTMESSAGES_H_
#define TEXTMESSAGES_H_

#include <list>

class Chat
{
public:
	struct TextMsg
	{
		static const int MAX_TEXT_LENGTH = 200;
		char text[MAX_TEXT_LENGTH];
		int dieTime;
	};

	typedef std::list<TextMsg> TextMsgList;

	Chat();
	void AddMessage(const char *text);
	const TextMsgList & GetMessages() const;
	void Update();

private:
	static const int MSG_DURATION = 5000;
	TextMsgList m_Messages;
};

inline const Chat::TextMsgList & Chat::GetMessages() const
{
	return m_Messages;
}

#endif /* TEXTMESSAGES_H_ */
