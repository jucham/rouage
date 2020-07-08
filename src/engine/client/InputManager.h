#ifndef INPUTMANAGER_H_INCLUDED
#define INPUTMANAGER_H_INCLUDED

//#include "/Library/Frameworks/SDL2.framework/Versions/A/Headers/SDL_keycode.h"
//#include "/Library/Frameworks/SDL2.framework/Versions/A/Headers/SDL_scancode.h"
#include <cstring>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_scancode.h>
//#include <SDL2/SDL_keycode.h>
#include <CEGUI/System.h>
#include <CEGUI/GUIContext.h>
#include <base/Types.h>
#include <engine/shared/Common.h>
//
#define SDL_BUTTON_WHEELUP 4
#define SDL_BUTTON_WHEELDOWN 5

//#define SDL_NUM_SCANCODES 512

class InputManager
{
private:
	static const int NUM_MOUSE_BUTTON = 5;

	bool 			 m_KeyPressed[SDL_NUM_SCANCODES];
	bool             m_MouseButtonPressed[NUM_MOUSE_BUTTON];
	int              m_iMouseX;
	int              m_iMouseY;
	int              m_axisValues[32];
	bool             m_bJoystickButtonPressed[32];
	bool             m_bExitGame;
	bool            *m_pInputs[NUM_INPUTS]; //
	unsigned char    m_NetInputs[NUM_INPUTS]; // input sent to server
	bool             m_KeyHit[SDL_NUM_SCANCODES];

	InputManager();
	InputManager(const InputManager &);
	InputManager & operator=(const InputManager &);

public:
	static InputManager & Instance() {
		static InputManager instance;
		return instance;
	}

	void CollectInputs();
	void SetKeyPressed(SDL_Scancode code, bool isPressed) {m_KeyPressed[code] = isPressed;}
	bool IsKeyPressed(SDL_Scancode code) const {return m_KeyPressed[code];}
	bool IsMouseButtonPressed(uint8_t mouseButton)  {return m_MouseButtonPressed[mouseButton-1];}
	int  MouseX()    {return m_iMouseX;}
	int  MouseY()    {return m_iMouseY;}
	bool ExitGame() const {return m_bExitGame;}

	void SetMouseButtonState(uint8_t button, bool state) {m_MouseButtonPressed[button-1] = state;}
	int GetAxisValues(int axisIdx) { return m_axisValues[axisIdx]; }
	bool IsJoystickButtonPressed(int buttonIdx)  {return m_bJoystickButtonPressed[buttonIdx];}
	void SetJoystickButtonState(int buttonIdx, bool actived) {m_bJoystickButtonPressed[buttonIdx] = actived;}
	bool IsExitInputTriggered() const { return ExitGame() /*|| IsKeyPressed(SDLK_ESCAPE)*/; }
	const unsigned char* GetInputs();
	void ResetInputs();

	int ButtonIndex(int sdlButtonIndex) {return sdlButtonIndex-1;}
	bool IsKeyHit(SDL_Scancode code) const {return m_KeyHit[code];}
	void ResetKeyHits() {std::memset(m_KeyHit, 0, sizeof(m_KeyHit));}

	static CEGUI::uint SDLKeyToCEGUIKey(SDL_Keycode key);
	void UI_InjectMouseDown( Uint8 button );
	void UI_InjectMouseUp( Uint8 button );
};


#endif // INPUTMANAGER_H_INCLUDED
