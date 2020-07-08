#ifndef MENUSTACK_H_
#define MENUSTACK_H_

#include <stack>
#include <CEGUI/CEGUI.h>

class Client;
class GameClient;

struct MenuEventHandler {
	Client *client;
	bool ChangeResolution(const CEGUI::EventArgs& ea);
	bool ChangeWindowMode(const CEGUI::EventArgs& ea);
	bool ValidationUpdateIPAddress(const CEGUI::EventArgs& ea);
	bool ValidationUpdatePlayername(const CEGUI::EventArgs& ea);

private:
	void UpdateEditboxColour(const CEGUI::RegexMatchStateEventArgs& ra,
			CEGUI::Editbox* eb);
};

class Menu {

	friend class MenuHandler;

private:
	CEGUI::Window *m_Window;
	static MenuEventHandler *m_MenuEventHandler;

public:
	CEGUI::Window * Load(const char *layoutFilename);
	void SetVisible(bool b);
	void Render();
};


class MenuStack {

public:
	bool Empty() const;
	void PushMenu(Menu *menu);
	bool PushMenu(const CEGUI::EventArgs &ea);
	void PopMenu();
	bool PopMenu(const CEGUI::EventArgs &ea);
	Menu * GetCurrentMenu() {return m_Stack.top();}

private:
	std::stack<Menu*> m_Stack;

};

class MenuHandler {

public:
	static MenuHandler & Instance() {
		static MenuHandler instance;
		if (!Menu::m_MenuEventHandler)
			Menu::m_MenuEventHandler = new MenuEventHandler();
		return instance;
	}

	Menu * LoadAndSetMenus(Client *client);
	Menu * LoadAndSetInGameMenus(GameClient *gc);
	void CreateUI(Client *client);
	void CreateInGameUI(GameClient *gc);
	void PositionAndSizeUI();
	void PositionAndSizeInGameUI();
	static void CreateTitle(const char *text, CEGUI::Window *w, const CEGUI::Font *font);
	static CEGUI::ButtonBase* CreateReturnButton(CEGUI::Window *w, Client *client);
	static void PositionAndSizeMenuTitle(CEGUI::Window *menuWindow, float yPos);
	static void PositionAndSizeReturnButton(CEGUI::Window *menuWindow, CEGUI::USize buttonSize);

private:
	MenuHandler();
	MenuHandler(const MenuHandler &);
	MenuHandler & operator=(const MenuHandler &);
	void ResizeAllMenus();
	bool SaveClientConfig(const CEGUI::EventArgs& ea);

	Menu *m_MainMenu;
	Menu *m_JoinMenu;
	Menu *m_SettingsMenu;
	Menu *m_ExitMenu;
	Menu *m_MainMenuInGame;

};

#endif /* MENUSTACK_H_ */
