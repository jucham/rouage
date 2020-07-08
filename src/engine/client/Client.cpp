#include "Client.h"
#include <cstdlib>
#include <base/DetectPlatform.h>
#include <base/Regulator.h>
#include <base/Time.h>
#include <base/utils.h>
#include <base/tri_logger.hpp>
#include <base/Stats.h>
#include <engine/shared/Net.h>
#include <engine/shared/MonitoringVars.h>
#include <engine/client/ClientSnapshotManager.h>
#include <game/client/ui/Monitor.h>
#include "ClientConfig.h"
#include "Console.h"

#if PLATFORM == PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


int main(int argc, char ** argv)
{

#if PLATFORM == PLATFORM_MAC
	ChangeDirectoryMacosx();
#endif

#if PLATFORM == PLATFORM_UNIX
	putenv("CEGUI_MODULE_DIR=\"lib/\"");
#endif

	try
	{
		TRI_LOG_ON();
		Time::init();
		srand ( (unsigned)time ( NULL ) );

		Address address;

		bool dumb = false;
		if (argc == 2 && IsEqualString("dumb", argv[1])) {
			printf("=== DUMB CLIENT ===\n");
			dumb = true;
		}

		if (!ClientConfig::Load()) {
			printf("Client configuration has failed, check your client.cfg\n");
			std::exit(0);
		}

		if (address == Address()) {
			printf("Server info loaded from client.cfg\n");
			address = Address(ClientConfig::DefaultServerIp, ClientConfig::DefaultServerPort);
		} else {
			printf("Server info loaded from command line\n");
		}

		Client client(address, dumb);

		if (!client.InitNetwork())
		{
			printf("failed to init client\n");
			return 1;
		}

		client.Run();
		ClientConfig::Save();
		ShutdownSockets();

	}
	catch(const std::exception& e)
	{
		printf("Exception : %s\n", e.what());
	}

	return 0;
}

Client::Client(const Address &serverAddress, bool dumb) :
				m_ConnectionInitialized(false),
				m_ConnectionFail(false),
				m_ConnectionFailType(0),
				m_State(STATE_OFFLINE),
				m_NetClient(),
				m_ServerAddress(serverAddress),
				m_GameClient(),
				m_LocalClientId(INVALID_CLIENT_ID),
				m_SnapRateAverage(0),
				m_SnapLatencyAverage(0),
				m_ReadyToExit(false),
				m_ExitRequested(false),
				m_LeaveGameRequested(false),
				m_Huffman(Huffman::LITERALS_FREQUENCIES),
				m_MenuStack(),
				m_InMenu(!dumb),
				m_Dumb(dumb)
{
	std::strcpy(m_LocalPlayerData.name, ClientConfig::PlayerName);
	m_LocalPlayerData.skinId = ClientConfig::SkinId;
}

bool Client::InitNetwork()
{
	bool success = InitializeSockets();
	if (!success) 
	{
		printf("failed to initialize sockets\n");
	}
	return success;
}

void Client::Run()
{
	Regulator renderRegulator(60, HERTZ_VALUE);
	Regulator networkRegulator(30, HERTZ_VALUE); //FIXME mettre 60hz pour eviter des d�lais d'update plus important que 1/30 ms
	Regulator consoleRegulator(30, HERTZ_VALUE);
	const int simToCatch = 10;
	int simCount = 0;
	int simTime[simToCatch];
	float avgNetTime = 0;

	Menu *mainMenu = MenuHandler::Instance().LoadAndSetMenus(this);
	m_MenuStack.PushMenu(mainMenu);

	while (IsRunning())
	{
		UpdateExitRequest();

		if(m_GameClient.GameLeft()) {
			m_GameClient.SetGameLeft(false);
			LeaveGame();
			m_InMenu = true;
		}

		if (m_ExitRequested) {
			LeaveGame();
			m_ReadyToExit = true;
		}

		ProcessCommands();
		InjectTimePulse();

		// --- RENDER MAIN MENU ---

		if (m_InMenu)
		{
			if (renderRegulator.isReady())
			{
				InputManager::Instance().CollectInputs();

				Menu *currentMenu = m_MenuStack.GetCurrentMenu();

				assert(currentMenu);
				m_GameClient.GetRenderer().RenderStartMenu(currentMenu);

				if (m_ExitRequested)
					m_ReadyToExit = true;
			}
		}
		
		// --- RUN GAME ---
		else {

			// --- PROCESS NETWORK ---

			// init the connection if not done
			if (!m_ConnectionInitialized) {
				m_ConnectionInitialized = true;
				if (!m_NetClient.StartConnection(m_ServerAddress)) {
					printf("failed to init connection\n");
					break;
				}
			}

			if (!m_ConnectionFail && networkRegulator.isReady())
			{
				int simStart = Time::ms();

				m_NetClient.UpdateFlowControl();
				if (!m_NetClient.CheckConnectionState()) {
					printf("client is unable to connect to server\n"); //TODO screen message
					break;
				}

				Send();

				NetPiece netPiece;
				while (m_NetClient.Receive(netPiece)) {
					ProcessPacket(netPiece);
					netPiece.Reset();
				}

				m_NetClient.UpdateConnection();

				// stats
				simTime[simCount++] = Time::ms() - simStart;
				if (simCount == simToCatch) {
					simCount = 0;
					int total = 0;
					for(int i = 0; i<simToCatch; i++)
						total += simTime[i];
					avgNetTime = (float)total / simToCatch;
				}
			}

			// --- RENDER GAME ---

			// count frame
			static int frameCount = 0;
			static int lastTime = 0;
			static float fps = 0;
			const int updateDelay = 500; // ms
			int timeElapsed = Time::ms() - lastTime;
			if (timeElapsed > updateDelay) {
				lastTime = Time::ms();
				fps =  frameCount * 1000.0f / timeElapsed;
				frameCount = 0;
			}

			if (renderRegulator.isReady())
			{
				InputManager::Instance().CollectInputs();

				if (m_State == STATE_ONLINE)
				{
					if (!m_Dumb) {
						Monitor::Instance().AddLine("Fps : %d", (int)(fps + 0.5f));
						Monitor::Instance().AddLine("Snapshot Frequency : %.1f Hz", 1/m_SnapRateAverage);
						Monitor::Instance().AddLine("Snapshot Latency : %.1f ms", m_SnapLatencyAverage);
						Monitor::Instance().AddLine("Network Iteration Time : %.1f ms", avgNetTime);

						m_GameClient.Update();
						m_GameClient.RenderGame();
						frameCount++;
					}
				}
				else
				{
					if (m_ConnectionFail) {

						if (m_ConnectionFailType == CF_INVALID_PROTOCOL) {
							m_GameClient.RenderScreenMessage("Client is not compatible with the protocol version of server");
						} else if (m_ConnectionFailType == CF_SERVER_FULL) {
							m_GameClient.RenderScreenMessage("Server is full");
						}

						if (InputManager::Instance().IsKeyHit(SDL_SCANCODE_ESCAPE)) {
							m_GameClient.GetRenderer().ActiveMenuWindow();
							m_InMenu = true;
						}
					} else /*if (m_State == STATE_CONNECTING)*/ {
						m_GameClient.RenderStartscreen();
						if (InputManager::Instance().IsKeyHit(SDL_SCANCODE_ESCAPE)) {
							LeaveGame();
							m_GameClient.GetRenderer().ActiveMenuWindow();
							m_InMenu = true;
						}
					}
				}
			}
		}

		wait_second(0.001f);
	}
}

void Client::SendInputs()
{
	NetPiece netPiece(AUTH_COM, NET_INPUTS);

	if (m_Dumb) {

		byte_t inputs[NUM_INPUTS] = {0};

		static int lastMove = 0;
		static bool goRight = true;
		static int period = 1000;
		if (Time::ms() - lastMove > period) {
			lastMove = Time::ms();
			period = RandInt(1000,3000);
			goRight = !goRight;
		}
		if (goRight)
			inputs[INPUT_RIGHT] = 1;
		else
			inputs[INPUT_LEFT] = 1;

		inputs[INPUT_JUMP] = (RandInt(0,30)==0?1:0);

		static int lastJet = 0;
		static bool jetpack = true;
		if (Time::ms() - lastJet > RandInt(750, 1500)) {
			lastJet = Time::ms();
			jetpack = !jetpack;
		}
		if (jetpack)
			inputs[INPUT_JETPACK] = 1;
		else
			inputs[INPUT_JETPACK] = 0;

		inputs[INPUT_ATTACK] = (RandInt(0,10)==0?1:0);
		inputs[INPUT_PREV_WEAPON] = (RandInt(0,30)==0?1:0);

		netPiece.AddBytes(inputs, NUM_INPUTS);

		static float angle = 0.0f;
		angle += 0.05f;
		if (angle > TwoPi)
			angle = 0;
		netPiece.AddFloat(angle);

	} else {
		// send no inputs when console is active
		if (Console::Instance().IsActive()
				|| m_GameClient.GetRenderer().FocusOnChat()
				|| m_GameClient.GetRenderer().InGameMenu()) {
			char noInputs[NUM_INPUTS] = {0};
			netPiece.AddBytes(noInputs, NUM_INPUTS);
		} else {
			netPiece.AddBytes(InputManager::Instance().GetInputs(), NUM_INPUTS);
		}
		netPiece.AddFloat(m_GameClient.GetLocalWeaponAngle());
	}

	bool sent = m_NetClient.Send(netPiece);
	if (sent)
	{
		InputManager::Instance().ResetInputs();
	}
}

void Client::SendProtocolVersion()
{
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	char msg[2];
	msg[0] = NETMSG_PROTOCOL_VERSION;
	msg[1] = PROTOCOL_VERSION;
	netPiece.AddBytes(msg, 2);
	m_NetClient.Send(netPiece);

	printf("Client protocol version: %d\n", PROTOCOL_VERSION);

	const byte_t *data = netPiece.GetData();
	assert(data[0] == NO_AUTH_COM && data[1] == NET_MSG && data[2] == NETMSG_PROTOCOL_VERSION && data[3] == PROTOCOL_VERSION);
}

void Client::Send()
{
	// client initialize communication
	if (m_State == STATE_OFFLINE)
	{
		SendProtocolVersion();
		printf("client sent protocol version\n");
		m_State = STATE_CONNECTING;
	}
	else if (m_State == STATE_CONNECTING)
	{
		//TODO send "Connecting" msg
	}
	else if (m_State == STATE_LOADING)
	{
		// useful if map should be downloaded
	}
	else if (m_State == STATE_ONLINE)
	{
		SendInputs();
	}
	else if (m_State == STATE_QUITING)
	{
		//TODO send a quit message
	}
}


void Client::ProcessPacket(NetPiece &netPiece)
{
	// reset data cursor
	netPiece.Rewind();

	int comType = netPiece.GetByte();
	int dataType = netPiece.GetByte();

	if (dataType == NET_MSG)
	{
		ProcessMessage(netPiece);
	}
	else if (dataType == NET_SNAPSHOT)
	{
		static int lastSnapTime = 0;
		static StatCalc sc(100);
		sc.AddValue(Time::ms() - lastSnapTime);
		lastSnapTime = Time::ms();
		if (sc.AllSampleCollected())
		{
			m_SnapRateAverage = sc.GetAverage();
			m_SnapLatencyAverage = m_SnapRateAverage-33;
			Clamp(m_SnapLatencyAverage, 0, 1000);
		}

		ProcessSnapshot(netPiece);
	}
	else
	{
		assert(false && "Data type of net piece is invalid\n");
	}

}

void Client::ProcessMessage(NetPiece& netPiece) {

	int msgType = netPiece.GetByte();
	switch (msgType) {

	case NETMSG_CONNECTION_FAIL:
	{
		m_ConnectionFail = true;
		m_ConnectionFailType = netPiece.GetByte();
		printf("received connection fail of type %d\n", m_ConnectionFailType);
		break;
	}

	case NETMSG_MAPINFO:
	{
		printf("info about map received\n");
		const char *pMapName = netPiece.GetString();

		bool loadSuccess = m_GameClient.LoadMap(pMapName);
		if (loadSuccess) {
			printf("map %s successfuly loaded\n", pMapName);
			SendReady();
			printf("ready signal sent\n");
		} else {
			//TODO downloading the map from the server
			printf("Unable to load the map %s\n", pMapName);
			exit(0);
		}

		break;
	}


	case NETMSG_READY:
	{
		printf("server confirm it is ready\n");

		m_SnapshotMgr.Reset();

		m_LocalClientId = netPiece.GetByte();
		m_GameClient.SetLocalClientId(m_LocalClientId);

		// get info of other clients
		int inGameClients = netPiece.GetByte();
		for (int i = 0; i < inGameClients; i++) {
			int clientId = netPiece.GetByte();
			std::strcpy(m_PlayerData[clientId].name, netPiece.GetString());
			m_PlayerData[clientId].skinId = netPiece.GetByte();
			CharacterData &cd = m_GameClient.GetCharacterData(clientId);
			std::strcpy(cd.name, m_PlayerData[clientId].name);
			cd.skinId = m_PlayerData[clientId].skinId;
			m_GameClient.UpdateClientData(clientId);
		}

		SendEnterInGame();
		printf("enter in game signal sent\n");

		m_State = STATE_ONLINE;

		break;
	}


	case NETMSG_CHAT:
	{
		const char *msg = netPiece.GetString();
		printf("[chat] %s\n", msg);
		m_GameClient.AddChatMessage(msg);
		break;
	}

	case NETMSG_CLIENT_DATA_CHANGE:
	{
		int clientId = netPiece.GetByte();
		printf("client %d has data changed\n", clientId);
		CharacterData& charData = m_GameClient.GetCharacterData(clientId);

		// name
		const char *pName = netPiece.GetString();
		std::strcpy(m_PlayerData[clientId].name, pName);
		std::strcpy(charData.name, m_PlayerData[clientId].name);

		// skin
		m_PlayerData[clientId].skinId = netPiece.GetByte();
		charData.skinId = m_PlayerData[clientId].skinId;
		m_GameClient.UpdateClientData(clientId);

		break;
	}

	case NETMSG_EVENTS:
	{
		EventData::numEvents = netPiece.GetByte();
		EventData *eventData = m_GameClient.GetEventData();
		for (int i = 0; i < EventData::numEvents; ++i) {
			byte_t values = netPiece.GetByte();
			eventData[i].ownerClientId = values >> 4;
			eventData[i].type = values & 0x0F;
			eventData[i].pos.x = netPiece.GetFloat();
			eventData[i].pos.y = netPiece.GetFloat();
		}
		break;
	}

	default:
		netPiece.Print();
		assert(false && "Message type no handled");
		break;
	}
}

//void Client::GetUncompressedSnapshot(NetDataArray &nda, NetPiece &np) const {
//
//	int numCompBits = np.GetInt16();
//	int numCompBytes = BitTricks::GetBytesNumber(numCompBits);
//
//	const byte_t *pCompData = np.GetBytes(numCompBytes);
//
//	static byte_t unCompData[Huffman::MAX_UNCOMPRESSED_PACKET_SIZE];
//	std::memset(unCompData, 0, Huffman::MAX_UNCOMPRESSED_PACKET_SIZE);
//
//	int numUncompByte = m_Huffman.Uncompress(pCompData, unCompData, numCompBits);
//
//	MonitoringVars::snapCompRatio = static_cast<double>(numUncompByte)/numCompBytes;
//
//	nda.AddBytes(unCompData, numUncompByte);
//	nda.Rewind();
//}

void Client::ProcessSnapshot(NetPiece& netPiece) {

	m_SnapshotMgr.DeltaUncompress(netPiece);
	const SnapshotData &snapData = m_SnapshotMgr.GetLastSnapshotData();

	// --- CHARACTERS ---
	for (int i = 0; i<MAX_CLIENTS; ++i)
	{
		CharacterData &charData = m_GameClient.GetCharacterData(i); // TODO recup en une fois

		if ( !snapData.netChars[i].exist && !charData.exist )
			continue;

		charData.exist = snapData.netChars[i].exist;
		byte_t states = snapData.netChars[i].states;
		charData.state = (states >> 4);
		charData.moveState = (states & 0x0F);
		charData.pos.x = snapData.netChars[i].x;
		charData.pos.y = snapData.netChars[i].y;
		charData.currentWeapon = snapData.netChars[i].weaponType;
		charData.weaponAngle = snapData.netChars[i].weaponAngle;
		charData.aAmmoCount[charData.currentWeapon] = snapData.netChars[i].ammo;
		charData.health = snapData.netChars[i].health;

		// player aura independent data
		charData.kills = snapData.netPlayers[i].kills;
		charData.deaths = snapData.netPlayers[i].deaths;
	}

	// --- PROJECTILES ---

	for (int i = 0; i < MAX_PROJECTILES; i++)
	{
		ProjectileData &projData = m_GameClient.GetProjectileData(i); // TODO recup en une fois

		if ( !snapData.netProjs[i].exist && !projData.exist ) //TODO peut eviter un get ren testant la premier expr
			continue;

		projData.exist = snapData.netProjs[i].exist;
		projData.weaponType = snapData.netProjs[i].type;
		projData.pos.x = snapData.netProjs[i].x;
		projData.pos.y = snapData.netProjs[i].y;
		projData.angle = snapData.netProjs[i].angle;
	}

	// --- TRIGGERS ---

	TriggerData *triggerData = m_GameClient.GetTriggerData();
	for (int i = 0; i < MAX_TRIGGERS; i++)
	{
		triggerData[i].exist = snapData.netTrigs[i].exist;
		triggerData[i].type = snapData.netTrigs[i].type;
		triggerData[i].subType = snapData.netTrigs[i].subType;
		triggerData[i].active = snapData.netTrigs[i].active;
		triggerData[i].pos.x = snapData.netTrigs[i].x;
		triggerData[i].pos.y = snapData.netTrigs[i].y;
	}

	MonitoringVars::snapSize = netPiece.GetInt16();
	MonitoringVars::snapSizeMin = netPiece.GetInt16();
	MonitoringVars::snapSizeMax = netPiece.GetInt16();
}

void Client::SendReady()
{
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	byte_t msgType = NETMSG_READY;
	netPiece.AddByte(&msgType);
	netPiece.AddString(m_LocalPlayerData.name);

	byte_t skinId = m_LocalPlayerData.skinId;
	netPiece.AddByte(&skinId);
	m_NetClient.Send(netPiece);
}

void Client::SendEnterInGame()
{
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	byte_t msgType = NETMSG_ENTER_IN_GAME;
	netPiece.AddByte(&msgType);
	m_NetClient.Send(netPiece);
}

void Client::SendLeave()
{
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	byte_t msgType = NETMSG_LEAVE;
	netPiece.AddByte(&msgType);
	m_NetClient.Send(netPiece);
	printf("client leaves server\n");
}

void Client::SendChat(const char *pMsg)
{
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	byte_t msgType = NETMSG_CHAT;
	netPiece.AddByte(&msgType);
	netPiece.AddString(pMsg);
	m_NetClient.Send(netPiece);
}

void Client::ProcessCommands() {

	CommandManager &cm = CommandManager::Instance();
	CommandManager::Command *cmd = 0;

	while (1)
	{
		cmd = cm.GetNextCommand();

		if (!cmd)
			break;

		switch (cmd->id)
		{
		case CommandManager::CMD_HELLOWORLD:
			Console::Instance().WriteLine("Hello World !");
			break;

		case CommandManager::CMD_HELP:
		{
			char allDesc[CommandManager::ALL_DESC_SIZE];
			cm.GetAllCommandDesc(allDesc);
			Console::Instance().Write(allDesc);
			break;
		}

		case CommandManager::CMD_SAY:
			//TODO check msg
			SendChat(cmd->args[0].value);
			break;

		case CommandManager::CMD_SHOW_CHAT:
			m_GameClient.GetRenderer().SetChatVisible(true);
			break;

		case CommandManager::CMD_MONITOR: {
			int monitoring = std::atoi(cmd->args[0].value);
			Clamp(monitoring, 0, 1);
			m_GameClient.SetMonitoring(monitoring);
			break;
		}

		default:
			assert(false);
			break;

		}
	}
}

bool Client::JoinGame(const CEGUI::EventArgs& ea) {
	const CEGUI::WindowEventArgs &wea = static_cast<const CEGUI::WindowEventArgs&>(ea);

	CEGUI::Editbox *serverAddress = static_cast<CEGUI::Editbox*>(wea.window->getParent()->getChild("ServerAddress"));
	const char *sa = serverAddress->getText().c_str();
	//TODO in function ScanAddress()
	int a, b, c, d;
	int port;
	if (sscanf(sa, "%d.%d.%d.%d:%d", &a, &b, &c, &d, &port)) {
		m_ServerAddress = Address(a, b, c, d, port);
		m_InMenu = false;
	} else {
		printf("IP address and/or port are invalid\n");
	}

	// update client config
	std::strcpy(m_LocalPlayerData.name, ClientConfig::PlayerName);
	m_LocalPlayerData.skinId = ClientConfig::SkinId;

	m_GameClient.GetRenderer().ActiveGameWindow();

	// avoid wrong input just after spawning
	InputManager::Instance().ResetInputs();

	return true;
}

bool Client::ExitGame(const CEGUI::EventArgs& ea) {
	m_ReadyToExit = true;
	return true;
}

void Client::InjectTimePulse() const
{
	static double lastTimePulse = 0.0f;
	double currentTimePulse = Time::sec();
	CEGUI::System::getSingleton().injectTimePulse( static_cast<float>(currentTimePulse - lastTimePulse ) ) ;
	lastTimePulse = currentTimePulse;
}

void Client::LeaveGame()
{
	SendLeave();
	m_NetClient.StopConnection();
	m_State = STATE_OFFLINE;
	m_LeaveGameRequested = false;
	m_ConnectionInitialized = false;
	m_GameClient.UnloadMap();
}
