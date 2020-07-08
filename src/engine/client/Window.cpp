#include "ClientConfig.h"
#include "Window.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <vector>
#include <engine/shared/AbsDim.h>
#include <base/debug.h>
#include <cassert>
#include <iostream>

int Window::m_FrameWidth = 1024;
int Window::m_FrameHeight = 768;
Vector2D Window::m_RealFrameDims = Vector2D(0,0);
bool Window::m_Fullscreen = false;
SDL_Window *Window::m_Window = 0;
SDL_GLContext Window::m_GLContext;
std::vector<Window::Resolution> Window::m_Resolutions;
const Window::Resolution *Window::m_CurrentResolution = 0;

Window::Window()
{
	m_RealFrameDims.x = ClientConfig::ScreenWidth;
	m_RealFrameDims.y = ClientConfig::ScreenHeight;
	m_Fullscreen = ClientConfig::Fullscreen;
	m_CurrentResolution = 0;
	Setup();
}

void Window::Setup()
{
	char caption[50];
	SetCaption(caption);
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
	GetAvailableResolutions();
	CreateSDLWindow(caption);
	CreateGLContext();
	OpenGLSetting();
}

void Window::SetCaption(char *caption) {
	char rev[20];
	std::sprintf(rev,"$Revision: 629 $");
	char *dollar;
	dollar=strchr(rev,'$');
	dollar[0]='(';
	dollar=strchr(rev,'$');
	dollar[0]=')';
	std::sprintf(caption, "Rouage 0.2 Alpha %s", rev);
}

void Window::CreateSDLWindow(const char *caption) {

	if (!ValidResolution()) {
		m_RealFrameDims.x = 800;
		m_RealFrameDims.y = 600;
	}

	m_CurrentResolution = GetResolution(m_RealFrameDims.x,m_RealFrameDims.y);

	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

	if (m_Fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;

	m_Window = SDL_CreateWindow(
			caption,
			50,
			50,
			m_RealFrameDims.x,
			m_RealFrameDims.y,
			flags);

	if (m_Window == 0) {
		printf("Could not create window: %s\n", SDL_GetError());
		exit(0);
	}
}

void Window::CreateGLContext() {
	m_GLContext = SDL_GL_CreateContext(m_Window);
	if (m_GLContext == NULL) {
		printf( "There was an error creating the OpenGL context!\n");
		exit(0);
	}

	const unsigned char *version = glGetString(GL_VERSION);
	if (version == 0){
		printf( "There was an error creating the OpenGL context!\n");
		exit(0);
	}

	SDL_GL_MakeCurrent(m_Window, m_GLContext);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, ?);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, ?);
}

void Window::OpenGLSetting() {
	InitExtensions();
	CheckExtension("GL_ARB_vertex_buffer_object");
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0);
	glEnable(GL_ALPHA_TEST);
	glDepthMask(0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Window::InitExtensions() const
{
	//glewExperimental = GL_TRUE; // for use of opengl > 2.0 only
	GLenum code = glewInit();
	if (code != GLEW_OK) {
		printf("glew initializing has failed: %s\n", glewGetErrorString(code));
		exit(1);
	}
}

void Window::CheckExtension(const char *name) const
{
	if (!glewIsSupported(name)) {
		printf("your video card does not support the opengl extension %s or hardware acceleration is not enabled\n", name);
		exit(1);
	}
}
void Window::Swap() {
	SDL_GL_SwapWindow(m_Window);
}

void Window::ToggleWindowMode() {
	m_Fullscreen = !m_Fullscreen;
	if (m_Fullscreen)
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN);
	else
		SDL_SetWindowFullscreen(m_Window, 0);
}

std::vector<Window::Resolution> & Window::GetResolutions() {
	return m_Resolutions;
}

void Window::GetAvailableResolutions() {

	int modeNumber = SDL_GetNumDisplayModes(0);
	if ( modeNumber < 0 ) {
		printf("getting number of video resolutions has failed (%s)\n", SDL_GetError());
		exit(0);
	}

	for (int i = 0 ; i < modeNumber ; i++ )
	{
		Resolution resolution;
		int error = SDL_GetDisplayMode(0,i,&resolution.displayMode);
		if ( error < 0 ) {
			printf("getting a video resolution has failed (%s)\n",SDL_GetError());
		} else {
			SetRatio(resolution);
			m_Resolutions.push_back(resolution);
		}
	}

	if (m_Resolutions.empty()) {
		printf("no display mode available");
		exit(0);
	}

}

void Window::SetRatio(Resolution &resolution) {
	float rawRatio = static_cast<float>(resolution.displayMode.w) / resolution.displayMode.h;
	int ratio = std::floor(1000 * rawRatio);
	if(1250 == ratio) {
		std::sprintf(resolution.ratio, "5:4");
	} else if(1333 == ratio) {
		std::sprintf(resolution.ratio, "4:3");
	} else if (1500 == ratio) {
		std::sprintf(resolution.ratio, "3:2");
	} else if (1600 == ratio) {
		std::sprintf(resolution.ratio, "16:10");
	} else if (1666 == ratio) {
		std::sprintf(resolution.ratio, "5:3");
	} else if (1777 == ratio) {
		std::sprintf(resolution.ratio, "16:9");
	} else {
		std::sprintf(resolution.ratio, "?:?");
	}
}

bool Window::ValidResolution() {
	int w = static_cast<int>(m_RealFrameDims.x);
	int h = static_cast<int>(m_RealFrameDims.y);
	for(size_t i = 0;  i < m_Resolutions.size(); ++i) {
		if (m_Resolutions[i].displayMode.w == w && m_Resolutions[i].displayMode.h == h) {
			return true;
		}
	}
	return false;
}

const Window::Resolution * Window::GetResolution(int w, int h) {
	for(int i=0; i<m_Resolutions.size(); ++i)
		if (m_Resolutions[i].displayMode.w == w && m_Resolutions[i].displayMode.h == h)
			return &m_Resolutions[i];
	return 0;
}

bool Window::IsWideScreenResolution() {
	if (IsEqualString("16:9", m_CurrentResolution->ratio))
		return true;
	return false;
}

void Window::ChangeResolution(int w, int h) {
	m_RealFrameDims.x = w;
	m_RealFrameDims.y = h;
	SDL_SetWindowSize(Window::m_Window, w, h);
	m_CurrentResolution = GetResolution(w,h);
	SDL_SetWindowDisplayMode(m_Window, &m_CurrentResolution->displayMode);

}
