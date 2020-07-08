#ifndef PANEL_H
#define PANEL_H

#include <base/DetectPlatform.h>
#include <engine/client/gfx/GFXAsset.h>


#if PLATFORM == PLATFORM_MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

class Panel
{
public:
	Panel( float wCenter,
			float hCenter,
			float wCorner,
			float hCorner);

	void Render(float x,
			float y,
			float r,
			float g,
			float b,
			float a);

	float Width() const;
	float Height() const;

protected:
	void Quad(float w, float h, float mx1, float my1, float mx2, float my2);

	GLuint  m_displayList;
	float   m_fWidth;
	float   m_fHeight;

};

inline float Panel::Width() const { return m_fWidth;}
inline float Panel::Height() const { return m_fHeight; }

#endif // PANEL_H
