//----------------------------------
// Window is a window SDL/OpenGL
//----------------------------------

#ifndef RENDERMANAGER_H_INCLUDED
#define RENDERMANAGER_H_INCLUDED

#include <vector>
#include <base/Vector2D.h>
#include <SDL2/SDL.h>

//TODO singleton
class Window
{
public:

	struct Resolution {
		SDL_DisplayMode displayMode;
		char ratio[6];
	};

	Window();
	static int FrameWidth() { return m_FrameWidth; }
	static int FrameHeight() { return m_FrameHeight; }
	static Vector2D FrameDims() { return Vector2D(m_FrameWidth, m_FrameHeight); }
	static void Swap();
	static void ToggleWindowMode();
	static std::vector<Resolution> & GetResolutions();
	static void ChangeResolution(int w, int h);
	static bool Fullscreen() {return m_Fullscreen;}
	static bool IsWideScreenResolution();
	static const Resolution * GetResolution(int w, int h);

	static int m_FrameWidth;
	static int m_FrameHeight;
	static Vector2D m_RealFrameDims;
	static SDL_Window *m_Window;
	static SDL_GLContext m_GLContext;
	static bool m_Fullscreen;
	static std::vector<Resolution> m_Resolutions;

private:
	void Setup();
	void SetCaption(char *caption);
	void GetAvailableResolutions();
	void SetRatio(Resolution &resolution);
	bool ValidResolution();
	void CreateSDLWindow(const char *caption);
	void CreateGLContext();
	void OpenGLSetting();
	void CheckExtension(const char *name) const;
	void InitExtensions() const;

	static const Resolution *m_CurrentResolution;
};

#endif // RENDERMANAGER_H_INCLUDED
