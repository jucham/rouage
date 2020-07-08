#include "Chat.h"
#include <iterator>
#include <cassert>
#include <base/Time.h>
#include  <engine/client/gfx/text/TextRender.h>
#include <cstring>

Chat::Chat() :
		m_Messages() {}

void Chat::AddMessage(const char *text)
{
	assert((int)std::strlen(text) <= TextMsg::MAX_TEXT_LENGTH);

	if (m_Messages.size() >= 5)
		m_Messages.pop_front();

	TextMsg msg;
	std::strcpy(msg.text, text);
	msg.dieTime = Time::ms() + MSG_DURATION;
	m_Messages.push_front(msg);
}

void Chat::Update()
{
	TextMsgList::iterator it = m_Messages.begin();
	while (it != m_Messages.end())
	{
		const TextMsg& msg = *it;
		if (Time::ms() > msg.dieTime)
			it = m_Messages.erase(it);
		else
			++it;
	}
}
