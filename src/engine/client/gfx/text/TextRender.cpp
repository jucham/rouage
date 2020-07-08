#include "TextRender.h"

const char *TextRender::FONT_FILENAMES[TextRender::NUM_FONT_TYPES] = {
		"data/fonts/NeoBulletin Limited Free Version.ttf",
		"data/fonts/DejaVuSans.ttf"
};

void TextRender::LoadFonts(float frameHeight)
{
	// 60 is magic number to get smaller font
	int fontSize = static_cast<int>(frameHeight / 60);
	for (int j = 0; j < NUM_FONT_SIZES; j++)
	{
		m_fonts[0][j].open(FONT_FILENAMES[0], fontSize);
		fontSize += 2;
	}

	fontSize = static_cast<int>(frameHeight / 64);
	for (int j = 0; j < NUM_FONT_SIZES; j++)
	{
		m_fonts[1][j].open(FONT_FILENAMES[1], fontSize);
		fontSize += 2;
	}
}

void TextRender::Render(float x, float y, const char *text, int fontSize, int fontType) const
{
	assert(fontSize >= 0 && fontSize < NUM_FONT_SIZES);
	assert(fontType >= 0 && fontType < NUM_FONT_TYPES);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable (GL_TEXTURE_2D);
	m_fonts[fontType][fontSize].drawText(x, y, text); //FIXME rendre tous les textes en meme temps !
	glDisable(GL_TEXTURE_2D);
}
