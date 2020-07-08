
#ifndef MONITORRENDER_H_
#define MONITORRENDER_H_

#include <engine/client/gfx/text/TextRender.h>

class Monitor
{

public:
	static Monitor & Instance()
	{
		static Monitor instance;
		return instance;
	}

	void Init(Vector2D offset)
	{
		m_Offset = offset;
		Reset();
	}

	void Reset()
	{
		m_CurLineIndex = 0;
	}

	void AddLine(const char *text, ...)
	{
		assert(m_CurLineIndex < MAX_LINE);
		assert(std::strlen(text) < MAX_STR_LENGTH-1);

		va_list ap;
		va_start(ap, text);
		vsprintf(m_TextLines[m_CurLineIndex], text, ap);
		va_end(ap);

		++m_CurLineIndex;
	}

	void Render(Vector2D camPos)
	{
		int numTexts = m_CurLineIndex + 1;
		for (int i = 0; i < numTexts; i++)
		{
			TextRender::Instance().Render(camPos.x + m_Offset.x, camPos.y + m_Offset.y - i*LINE_HEIGHT, m_TextLines[i], 0, TextRender::FONT_SYSTEM);
		}
	}

private:
	Monitor() {}
	Monitor(const Monitor &);
	Monitor & operator=(const Monitor &);

	Vector2D m_Offset;
	static const int LINE_HEIGHT = 20;
	static const int MAX_LINE = 30;
	static const int MAX_STR_LENGTH = 128;
	char m_TextLines[MAX_LINE][MAX_STR_LENGTH];
	int m_CurLineIndex;

};

#endif /* MONITORRENDER_H_ */
