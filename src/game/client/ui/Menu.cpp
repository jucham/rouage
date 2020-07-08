#include "Menu.h"
#include <vector>
#include <engine/client/Client.h>
#include <engine/client/Window.h>
#include <engine/client/ClientConfig.h>
#include <game/client/GameClient.h>


//========================================================
// MENU
//========================================================

MenuEventHandler * Menu::m_MenuEventHandler = 0;

CEGUI::Window * Menu::Load(const char *layoutFilename)
{
	CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::DefaultWindow *window = static_cast<CEGUI::DefaultWindow*>(winMgr.loadLayoutFromFile(layoutFilename));
	assert(window);
	m_Window = window;
	SetVisible(false);
	printf("%s loaded\n", m_Window->getName().c_str());
	return window;
}

void Menu::SetVisible(bool b) {
	m_Window->setVisible(b);

	if (m_Window->isVisible()) {
		m_Window->activate();
	} else {
		m_Window->deactivate();
	}
}

void Menu::Render() {
	m_Window->render();
}

//========================================================
// MENU STACK
//========================================================

bool MenuStack::Empty() const {
	return m_Stack.empty();
}

void MenuStack::PushMenu(Menu *menu)
{
	if (!m_Stack.empty()) {
		Menu *oldMenu = m_Stack.top();
		oldMenu->SetVisible(false);
	}

	menu->SetVisible(true);
	m_Stack.push(menu);
}

bool MenuStack::PushMenu(const CEGUI::EventArgs &ea)
{
	const CEGUI::WindowEventArgs & wea = static_cast<const CEGUI::WindowEventArgs&>(ea);
	CEGUI::PushButton *button = static_cast<CEGUI::PushButton*>(wea.window);
	Menu *newMenu = static_cast<Menu*>(button->getUserData());
	PushMenu(newMenu);
	return true;
}

void MenuStack::PopMenu()
{
	Menu *oldMenu = m_Stack.top();
	oldMenu->SetVisible(false);
	m_Stack.pop();

	if (!m_Stack.empty()) {
		Menu *newMenu = m_Stack.top();
		newMenu->SetVisible(true);
	}
}

bool MenuStack::PopMenu(const CEGUI::EventArgs &ea) {
	PopMenu();
	return true;
}


//========================================================
// MENU LOADER
//========================================================

MenuHandler::MenuHandler() :
								m_MainMenu(new Menu()),
								m_JoinMenu(new Menu()),
								m_SettingsMenu(new Menu()),
								m_ExitMenu(new Menu()),
								m_MainMenuInGame(new Menu())
{}

void MenuHandler::CreateTitle(const char *text, CEGUI::Window *w, const CEGUI::Font *font) {
	// shadow
	CEGUI::Window *label = w->createChild("RouageLook/Label");
	label->setName(w->getName() + "TitleShadow");
	label->setFont(font);
	char str[50];
	std::sprintf(str, "[colour='FF000000']%s", text);
	label->setText(str);
	label->setProperty("HorizontalAlignment", "Centre");
	// text
	label = w->createChild("RouageLook/Label");
	label->setName(w->getName() + "TitleText");
	label->setFont(font);
	std::sprintf(str, "[colour='FFF1F1DA']%s", text);
	label->setText(str);
	label->setProperty("HorizontalAlignment", "Centre");
}

CEGUI::ButtonBase* MenuHandler::CreateReturnButton(CEGUI::Window *w, Client *client) {
	CEGUI::ButtonBase* button = static_cast<CEGUI::ButtonBase*>(w->createChild("RouageLook/Button"));
	button->setName(w->getName() + "Return");
	button->setText("Return");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuStack::PopMenu, &client->m_MenuStack));
	button->setProperty("HorizontalAlignment", "Centre");
//	std::printf("return button <%s> created in <%s>\n", button->getName().c_str(), w->getName().c_str());
	return button;
}

void MenuHandler::CreateUI(Client *client) {

	Menu::m_MenuEventHandler->client = client;

	CEGUI::Window *window = 0;
	CEGUI::ButtonBase *button = 0;
	const CEGUI::Font *menuTitleFont = client->m_GameClient.GetRenderer().GetCEGUIFont(Renderer::CEGUI_FONT_MENU_TITLE);

	//-----------------------------------------------------------------------------
	// MAIN
	//-----------------------------------------------------------------------------

	window = m_MainMenu->Load("MainMenu.layout");
	CreateTitle("ROUAGE", window, menuTitleFont);

	// join
	button = static_cast<CEGUI::ButtonBase*>(window->getChild("JoinGame")); //TODO besoin d'tre button ???
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuStack::PushMenu, &client->m_MenuStack));
	button->setUserData(m_JoinMenu);

	// settings
	button = static_cast<CEGUI::ButtonBase*>(window->getChild("Settings"));
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuStack::PushMenu, &client->m_MenuStack));
	button->setUserData(m_SettingsMenu);

	// exit
	button = static_cast<CEGUI::ButtonBase*>(window->getChild("ExitGame"));
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuStack::PushMenu, &client->m_MenuStack));
	button->setUserData(m_ExitMenu);

	client->m_GameClient.GetRenderer().AddWindowInMenu(window);

	//-----------------------------------------------------------------------------
	// JOIN
	//-----------------------------------------------------------------------------

	window = m_JoinMenu->Load("JoinMenu.layout");
	CreateTitle("Join", window, menuTitleFont);

	// join button
	button = static_cast<CEGUI::ButtonBase*>(window->getChild("Join"));
	button->setText("Join");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::JoinGame, client));

	// server address edit box
	CEGUI::Editbox *serverAddress = static_cast<CEGUI::Editbox*>(window->getChild("ServerAddress"));
	serverAddress->setUserData(button);
	std::string ip = "(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])";
	std::string port = "(102[4-9]|10[3-9][0-9]|1[1-9][0-9]{2}|[2-9][0-9]{3}|[1-5][0-9]{4}|6[1-5]{2}[1-3][1-5])";
	serverAddress->setValidationString("^"+ip+":"+port+"$");
	char servAddr[20];
	client->m_ServerAddress.ToString(servAddr);
	serverAddress->setText(servAddr);
	serverAddress->subscribeEvent(CEGUI::Editbox::EventTextValidityChanged, CEGUI::Event::Subscriber(&MenuEventHandler::ValidationUpdateIPAddress, Menu::m_MenuEventHandler));

	button = CreateReturnButton(window, client);
	button->subscribeEvent(CEGUI::ButtonBase::EventMouseClick, CEGUI::Event::Subscriber(&MenuHandler::SaveClientConfig, this));

	client->m_GameClient.GetRenderer().AddWindowInMenu(window);

	//-----------------------------------------------------------------------------
	// SETTINGS
	//-----------------------------------------------------------------------------

	window = m_SettingsMenu->Load("SettingsMenu.layout");
	CreateTitle("Settings", window, menuTitleFont);

	CEGUI::TabControl* tc = static_cast<CEGUI::TabControl*>(window->getChild("TabControl"));

	// --- player tab ---
	CEGUI::Window* tab = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("SettingsPlayerTab.layout");
	tab->setText("Player");
	CEGUI::Editbox *editbox = static_cast<CEGUI::Editbox*>(tab->getChild("Hbox/PlayerName"));
	editbox->setText(ClientConfig::PlayerName);
	editbox->setMaxTextLength(15);

	char maxPlayerNameLength[4] = {0};
	std::sprintf(maxPlayerNameLength, "%d", MAX_PLAYERNAME_LENGTH);
	std::string playerNameRegex = "^\\w{1," + std::string(maxPlayerNameLength) + "}$";
	editbox->setValidationString(playerNameRegex);
	editbox->subscribeEvent(CEGUI::Editbox::EventTextValidityChanged, CEGUI::Event::Subscriber(&MenuEventHandler::ValidationUpdatePlayername, Menu::m_MenuEventHandler));
	tc->addTab(tab);

	// --- video tab ---
	// resolutions
	tab = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("SettingsVideoTab.layout");
	tab->setText("Video");
	CEGUI::Listbox* listbox = static_cast<CEGUI::Listbox*>(tab->getChild("ResolutionsList"));
	listbox->setMultiselectEnabled(false);
	std::vector<Window::Resolution> &resolutions = Window::GetResolutions();
	for (size_t i = 0; i<resolutions.size(); ++i) {
		char resolutionStr[50];
		std::sprintf(resolutionStr, "%d x %d (%s) at %d Hz", resolutions[i].displayMode.w, resolutions[i].displayMode.h, resolutions[i].ratio, resolutions[i].displayMode.refresh_rate);
		CEGUI::ListboxTextItem* itemListbox = new CEGUI::ListboxTextItem(resolutionStr, i);
		itemListbox->setSelectionBrushImage("RouageLook/MultiListSelectionBrush");
		itemListbox->setUserData(new Vector2D(resolutions[i].displayMode.w, resolutions[i].displayMode.h));
		listbox->addItem(itemListbox);

		if (resolutions[i].displayMode.w == Window::m_RealFrameDims.x && resolutions[i].displayMode.h == Window::m_RealFrameDims.y) {
			listbox->setItemSelectState(itemListbox, true);
			listbox->ensureItemIsVisible(itemListbox);
		}
	}
	listbox->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&MenuEventHandler::ChangeResolution, Menu::m_MenuEventHandler));

	// fullscreen
	CEGUI::ToggleButton *checkBox = static_cast<CEGUI::ToggleButton*>(tab->getChild("FullscreenCheckBox"));
	checkBox->setSelected(Window::Fullscreen());
	checkBox->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&MenuEventHandler::ChangeWindowMode, Menu::m_MenuEventHandler));

	tc->addTab(tab);

	button = CreateReturnButton(window, client);
	button->subscribeEvent(CEGUI::ButtonBase::EventMouseClick, CEGUI::Event::Subscriber(&MenuHandler::SaveClientConfig, this));

	client->m_GameClient.GetRenderer().AddWindowInMenu(window);

	//-----------------------------------------------------------------------------
	// EXIT
	//-----------------------------------------------------------------------------

	window = m_ExitMenu->Load("ExitMenu.layout");
	CreateTitle("Exit", window, menuTitleFont);

	// question
	CEGUI::Window *question = static_cast<CEGUI::Window*>(window->getChild("Question"));
	question->setText("Are you sure to exit ?");

	// yes button
	button = static_cast<CEGUI::ButtonBase*>(window->getChild("Yes"));
	button->setText("Yes");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::ExitGame, client));

	// no button
	button = static_cast<CEGUI::ButtonBase*>(window->getChild("No"));
	button->setText("No");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuStack::PopMenu, &client->m_MenuStack));

	client->m_GameClient.GetRenderer().AddWindowInMenu(window);
}


Menu * MenuHandler::LoadAndSetMenus(Client *client)
{
	CreateUI(client);
	PositionAndSizeUI();
	return m_MainMenu;
}

bool MenuHandler::SaveClientConfig(const CEGUI::EventArgs& ea) {

	// resolution
	CEGUI::Listbox* resolutions = static_cast<CEGUI::Listbox*>(m_SettingsMenu->m_Window->getChild("TabControl/SettingsVideoWindow/ResolutionsList"));
	CEGUI::ListboxItem *resolution = resolutions->getFirstSelectedItem();
	Vector2D *dims = static_cast<Vector2D*>(resolution->getUserData());
	ClientConfig::ScreenWidth = dims->x;
	ClientConfig::ScreenHeight = dims->y;

	// fullscreen
	CEGUI::ToggleButton *checkBox = static_cast<CEGUI::ToggleButton*>(m_SettingsMenu->m_Window->getChild("TabControl/SettingsVideoWindow/FullscreenCheckBox"));
	ClientConfig::Fullscreen =checkBox->isSelected();

	// player name
	CEGUI::Editbox *playerName = static_cast<CEGUI::Editbox*>(m_SettingsMenu->m_Window->getChild("TabControl/SettingsPlayerWindow/Hbox/PlayerName"));
	std::sprintf(ClientConfig::PlayerName, "%s", playerName->getText().c_str());

	// address and port
	CEGUI::Editbox *serverAddress = static_cast<CEGUI::Editbox*>(m_JoinMenu->m_Window->getChild("ServerAddress"));
	std::string addressPort = serverAddress->getText().c_str();
	int addressSize = addressPort.find_first_of(':');
	std::string address = addressPort.substr(0, addressSize);
	ClientConfig::SetDefaultServerIp(address);
	int portSize = addressPort.size() - addressSize;
	std::string port = addressPort.substr(addressSize+1, portSize);
	ClientConfig::DefaultServerPort = std::atoi(port.c_str());

	return true;
}

void MenuHandler::PositionAndSizeMenuTitle(CEGUI::Window *menuWindow, float yPos)
{
	CEGUI::Window *label = menuWindow->getChild(menuWindow->getName() + "TitleShadow");
	label->setSize(CEGUI::USize(cegui_reldim(1), cegui_reldim(0.15f)));
	float offset = Window::m_RealFrameDims.x*0.005f;
	label->setXPosition(CEGUI::UDim(0,offset));
	label->setYPosition(CEGUI::UDim(0, yPos + offset));
	label = menuWindow->getChild(menuWindow->getName() + "TitleText");
	label->setSize(CEGUI::USize(cegui_reldim(1), cegui_reldim(0.15f)));
	label->setYPosition(CEGUI::UDim(0, yPos));
}

void MenuHandler::PositionAndSizeReturnButton(CEGUI::Window *menuWindow, CEGUI::USize buttonSize)
{
	CEGUI::Window *button = menuWindow->getChild(menuWindow->getName() + "Return");
	float bottomMinusButtonSize = Window::m_RealFrameDims.y - buttonSize.d_height.d_offset * 1.5f;
	button->setSize(buttonSize);
	button->setYPosition(CEGUI::UDim(0, bottomMinusButtonSize));
}

void MenuHandler::PositionAndSizeUI()
{
	CEGUI::Window *window = 0;
	CEGUI::USize buttonSize (CEGUI::UDim(0, Window::m_RealFrameDims.x * 0.2f), CEGUI::UDim(0, Window::m_RealFrameDims.y * 0.075f));
	float topToTitleDist = Window::m_RealFrameDims.y * 0.1f;
	float titleToFirstItemDist = topToTitleDist + Window::m_RealFrameDims.y * 0.15f;
	float buttonToButtonDist = Window::m_RealFrameDims.y * 0.1f;
	float currentYPos = 0.0f;

	//-----------------------------------------------------------------------------
	// MAIN
	//-----------------------------------------------------------------------------

	// title
	currentYPos += topToTitleDist;
	PositionAndSizeMenuTitle(m_MainMenu->m_Window, currentYPos);

	// join
	currentYPos += titleToFirstItemDist;
	window = m_MainMenu->m_Window->getChild("JoinGame");
	window->setSize(buttonSize);
	window->setYPosition(CEGUI::UDim(0,currentYPos));


	// settings
	currentYPos += buttonToButtonDist;
	window = m_MainMenu->m_Window->getChild("Settings");
	window->setSize(buttonSize);
	window->setYPosition(CEGUI::UDim(0,currentYPos));

	// exit
	currentYPos += buttonToButtonDist;
	window = m_MainMenu->m_Window->getChild("ExitGame");
	window->setSize(buttonSize);
	window->setYPosition(CEGUI::UDim(0,currentYPos));


	//-----------------------------------------------------------------------------
	// JOIN
	//-----------------------------------------------------------------------------

	currentYPos = 0.0f;
	currentYPos += topToTitleDist;
	PositionAndSizeMenuTitle(m_JoinMenu->m_Window, currentYPos);
	currentYPos += titleToFirstItemDist;

	// edit box
	window = m_JoinMenu->m_Window->getChild("ServerAddress");
	window->setYPosition(CEGUI::UDim(0,currentYPos));

	currentYPos += buttonToButtonDist;

	window = m_JoinMenu->m_Window->getChild(("Join"));
	window->setSize(buttonSize);
	window->setYPosition(CEGUI::UDim(0,currentYPos));

	currentYPos += buttonToButtonDist;

	PositionAndSizeReturnButton(m_JoinMenu->m_Window, buttonSize);

	//-----------------------------------------------------------------------------
	// SETTINGS
	//-----------------------------------------------------------------------------

	currentYPos = 0.0f;
	currentYPos += topToTitleDist;
	PositionAndSizeMenuTitle(m_SettingsMenu->m_Window, currentYPos);
	currentYPos += titleToFirstItemDist;

	//	CEGUI::TabControl* tc = static_cast<CEGUI::TabControl*>(m_SettingsMenu->m_Window->getChild("TabControl"));
	//	tc->setAu


	PositionAndSizeReturnButton(m_SettingsMenu->m_Window, buttonSize);

	//-----------------------------------------------------------------------------
	// EXIT
	//-----------------------------------------------------------------------------

	currentYPos = 0.0f;
	currentYPos += topToTitleDist;
	PositionAndSizeMenuTitle(m_ExitMenu->m_Window, currentYPos);
	currentYPos += titleToFirstItemDist;

	window = m_ExitMenu->m_Window->getChild(("Question"));
	window->setYPosition(CEGUI::UDim(0,currentYPos));

	currentYPos += buttonToButtonDist;

	window = m_ExitMenu->m_Window->getChild(("Yes"));
	window->setSize(buttonSize);
	window->setYPosition(CEGUI::UDim(0,currentYPos));

	currentYPos += buttonToButtonDist;

	window = m_ExitMenu->m_Window->getChild(("No"));
	window->setSize(buttonSize);
	window->setYPosition(CEGUI::UDim(0,currentYPos));

	m_SettingsMenu->m_Window->update(1); //notifyScreenAreaChanged();
}

Menu * MenuHandler::LoadAndSetInGameMenus(GameClient *gc)
{
	m_MainMenuInGame = new Menu();
	CreateInGameUI(gc);
	PositionAndSizeInGameUI();
	return m_MainMenuInGame;
}

void MenuHandler::CreateInGameUI(GameClient *gc)
{
	CEGUI::Window *window = 0;
	CEGUI::ButtonBase *button = 0;

	//-----------------------------------------------------------------------------
	// MAIN
	//-----------------------------------------------------------------------------

	window = m_MainMenuInGame->Load("MainInGameMenu.layout");

	// resume
	button = static_cast<CEGUI::ButtonBase*>(window->getChild("ResumeGame"));
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameClient::ResumeGame, gc));

	//	// settings
	//	button = static_cast<CEGUI::ButtonBase*>(window->getChild("Settings"));
	//	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuStack::PushMenu, &gc->m_MenuStack));
	//	button->setUserData(m_SettingsMenu);

	// leave
	button = static_cast<CEGUI::ButtonBase*>(window->getChild("LeaveGame"));
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameClient::LeaveGame, gc));

	gc->GetRenderer().AddWindowInGame(window);

}

void MenuHandler::PositionAndSizeInGameUI()
{
	CEGUI::Window *window = 0;
	CEGUI::USize buttonSize (CEGUI::UDim(0, Window::m_RealFrameDims.x * 0.2f), CEGUI::UDim(0, Window::m_RealFrameDims.y * 0.075f));
	float topToTitleDist = Window::m_RealFrameDims.y * 0.1f;
	float titleToFirstItemDist = topToTitleDist + Window::m_RealFrameDims.y * 0.15f;
	float buttonToButtonDist = Window::m_RealFrameDims.y * 0.1f;
	float currentYPos = 0.0f;

	//-----------------------------------------------------------------------------
	// MAIN
	//-----------------------------------------------------------------------------

	currentYPos += topToTitleDist;
	currentYPos += titleToFirstItemDist;

	// resume
	window = m_MainMenuInGame->m_Window->getChild("ResumeGame");
	window->setSize(buttonSize);
	window->setYPosition(CEGUI::UDim(0,currentYPos));

	currentYPos += buttonToButtonDist;

	// leave
	window = m_MainMenuInGame->m_Window->getChild("LeaveGame");
	window->setSize(buttonSize);
	window->setYPosition(CEGUI::UDim(0,currentYPos));
}

bool MenuEventHandler::ChangeResolution(const CEGUI::EventArgs& ea) {
	const CEGUI::WindowEventArgs &wea = static_cast<const CEGUI::WindowEventArgs&>(ea);
	const CEGUI::Listbox *lb = static_cast<const CEGUI::Listbox*>(wea.window);
	CEGUI::ListboxItem *lbi = lb->getFirstSelectedItem();
	Vector2D *resolution = static_cast<Vector2D*>(lbi->getUserData());
	client->m_GameClient.GetRenderer().ChangeResolution(resolution->x,resolution->y);
	MenuHandler::Instance().PositionAndSizeUI();
	MenuHandler::Instance().PositionAndSizeInGameUI();
	CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Sizef(resolution->x, resolution->y));
	return true;
}

bool MenuEventHandler::ChangeWindowMode(const CEGUI::EventArgs& ea) {
	Window::ToggleWindowMode();
	return true;
}

void MenuEventHandler::UpdateEditboxColour(
		const CEGUI::RegexMatchStateEventArgs& ra, CEGUI::Editbox* eb) {
	switch (ra.matchState) {
	case CEGUI::RegexMatcher::MS_INVALID:
		eb->setProperty("NormalTextColour", "FFFF5050");
		break;
	case CEGUI::RegexMatcher::MS_PARTIAL:
		eb->setProperty("NormalTextColour", "FFB2B2B2");
		break;
	case CEGUI::RegexMatcher::MS_VALID:
		eb->setProperty("NormalTextColour", "FFFFFFFF");
		break;
	}
}

bool MenuEventHandler::ValidationUpdateIPAddress(const CEGUI::EventArgs& ea)
{
	const CEGUI::RegexMatchStateEventArgs& ra(static_cast<const CEGUI::RegexMatchStateEventArgs&>(ea));
	CEGUI::Editbox *eb = static_cast<CEGUI::Editbox*>(ra.window);
	UpdateEditboxColour(ra, eb);

	CEGUI::ButtonBase *joinButton = static_cast<CEGUI::ButtonBase*>(eb->getUserData());
	if (ra.matchState == CEGUI::RegexMatcher::MS_VALID)
		joinButton->setDisabled(false);
	else
		joinButton->setDisabled(true);
	return true;
}

bool MenuEventHandler::ValidationUpdatePlayername(const CEGUI::EventArgs& ea)
{
	const CEGUI::RegexMatchStateEventArgs& ra(static_cast<const CEGUI::RegexMatchStateEventArgs&>(ea));
	CEGUI::Editbox *eb = static_cast<CEGUI::Editbox*>(ra.window);
	UpdateEditboxColour(ra, eb);
	return true;
}
