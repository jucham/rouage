#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include <list>
#include <vector>
#include "TextString.h"
#include "GLFT_Font.h"

class TextRender
{

public:

	enum {
			FONT_GAME,
			FONT_SYSTEM,
			NUM_FONT_TYPES
		};

	static TextRender & Instance()
	{
		static TextRender instance;
		return instance;
	}

	void LoadFonts(float frameHeight);
	void Render(float x, float y, const char *text, int fontSize, int fontType = FONT_GAME) const;
	const GLFT_Font& GetFont(int fontSize, int fontType = FONT_GAME) const;

private:

	enum
	{
		NUM_FONT_SIZES = 10
	};

	static const char *FONT_FILENAMES[NUM_FONT_TYPES];

	TextRender() {}
	TextRender(const TextRender &);
	TextRender & operator=(const TextRender &);

	GLFT_Font m_fonts[NUM_FONT_TYPES][NUM_FONT_SIZES];
};

inline const GLFT_Font& TextRender::GetFont(int fontSize, int fontType) const
{
	assert(fontSize >= 0 && fontSize < NUM_FONT_SIZES);
	assert(fontType >= 0 && fontType < NUM_FONT_TYPES);

	return m_fonts[fontType][fontSize];
}

#endif // TEXTMANAGER_H
