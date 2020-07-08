/*
 *  Server : handle clients, simulation, etc...
 *
 *     1. Sample clock to find start time
 *     2. Read client messages (input, connection, etc.) from network
 *     3. Execute client user input messages
 *     4. Simulate server-controlled objects using simulation time from last full pass
 *     5. For each connected client, package up visible objects/world state and send to client
 *     6. Sample clock to find end time
 *     7. End time minus start time is the simulation time for the next frame
 *
 */

#include <cstdio>
#include "Server.h"
#include <base/debug.h>
#include <base/Time.h>
#include <game/server/Map.h>
#include <base/tri_logger.hpp>
#include <engine/shared/MonitoringVars.h>

//#define USE_GPROF
#ifdef USE_GPROF
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
void sigUsr1Handler(int sig)
{
	fprintf(stderr, "Exiting on SIGUSR1\n");
	void (*_mcleanup)(void);
	_mcleanup = (void (*)(void))dlsym(RTLD_DEFAULT, "_mcleanup");
	if (_mcleanup == NULL)
		fprintf(stderr, "Unable to find gprof exit hook\n");
	else _mcleanup();
	_exit(0);
}
#endif


#if PLATFORM == PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef USE_GPROF
	signal(SIGUSR1, sigUsr1Handler); // send SIGUSR1 when use gprof
#endif

#if PLATFORM == PLATFORM_MAC
	ChangeDirectoryMacosx();
#endif

	try {
		TRI_LOG_ON();
		Server server;
		if (!server.Init())
			return 1;
		server.Run();
		server.Shutdown();
	}
	catch(const std::exception& e) {
		printf("Exception : %s\n", e.what());
	}

	return 0;
}

FILE *Server::logfile = 0;

Server::Server() :
				m_Clients(NetServer::NUM_SLOTS),
				m_NetServer(),
				m_GameServer(*this),
				m_CurrentTick(0)
{
	for (int i=0; i<MAX_CLIENTS; ++i)
		m_SnapshotMgrs[i].SetClientId(i);
	ServerSnapshotManager::SetServer(this);
}

void Server::ProcessPacket(NetPiece &netPiece)
{
	int clientId = netPiece.GetClientId();

	// neither slot available, don't process packet
	if (clientId == INVALID_CLIENT_ID)
		return;

	if (netPiece.isFirstPacket())
		m_Clients[clientId].Init();

	netPiece.Rewind();

	int comType = netPiece.GetByte();
	int dataType = netPiece.GetByte();

	if (dataType == NET_MSG)
		ProcessMessage(clientId, netPiece);
	else if (dataType == NET_INPUTS)
		if (IsClientInGame(clientId))
			ProcessInputs(clientId, netPiece);
}

void Server::ProcessMessage(int clientId, NetPiece& netPiece) {

	int msgType = netPiece.GetByte();
	switch (msgType)
	{
	case NETMSG_PROTOCOL_VERSION:
		if (m_Clients[clientId].IsAuth())
		{
			m_Clients[clientId].SetStateAsConnecting();
			int version = netPiece.GetByte();

			printf("client protocol version: %d\n", version);
			printf("server protocol version: %d\n", PROTOCOL_VERSION);

			if (version == PROTOCOL_VERSION) {

				printf("client use the right protocol version\n");
				if (m_GameServer.IsServerFull())
				{
					HandleConnectionFail(clientId, CF_SERVER_FULL);
					printf("reject client as no slot are available\n");
				}
				else
				{
					SendMapInfo(clientId);
					printf("info on map sent to client %d\n", clientId);
				}
			} else {
				HandleConnectionFail(clientId, CF_INVALID_PROTOCOL);
			}
		}
		break;

	case NETMSG_READY:
		if (m_Clients[clientId].IsConnecting()) {
			m_Clients[clientId].SetStateAsReady();

			std::strcpy(m_Clients[clientId].aName, netPiece.GetString()); //TODO make safe
			m_Clients[clientId].skinId = netPiece.GetByte();

			bool infosOk = true; // TODO check all infos
			if (infosOk) {
				printf("ready message received from client %d\n", clientId);
				SendReady(clientId);
				printf("confirm ready to client %d\n", clientId);
			}
		}
		break;

	case NETMSG_ENTER_IN_GAME:
		if (m_Clients[clientId].IsReady()) {
			m_Clients[clientId].SetStateAsInGame();
			printf("enter in game message received from client %d\n", clientId);
			SendClientEnterGame(clientId);
			printf("confirm enter in game to client %d\n", clientId);
		}
		break;

	case NETMSG_LEAVE:
		assert( clientId >= 0 && clientId < MAX_CLIENTS);
		if (m_Clients[clientId].IsInGame()) {
			m_NetServer.Disconnect(clientId);
			printf("client %d left the game\n", clientId);
		}

		SendClientLeaveGame(clientId);
		printf("notify all clients that client %d left the game\n", clientId);
		break;

	case NETMSG_CHAT: {
		const char *pMsg = netPiece.GetString();
		//TODO truncate message if too long
		char msg[1024];
		std::sprintf(msg, "%s : %s", m_Clients[clientId].aName, pMsg);

		printf("[chat] %s\n", msg);

		NetPiece netPiece(NO_AUTH_COM, NET_MSG); //TODO ajouter le client id pour pouvoir afficher le nom du client
		byte_t msgType = NETMSG_CHAT;
		netPiece.AddByte(&msgType);
		netPiece.AddString(msg);
		SendMsgAllClients(netPiece);

		break;
	}

	default:
		assert(false && "Message type no handled");
		break;
	}
}

void Server::ProcessInputs(int clientId, NetPiece& netPiece) {
	const byte_t* pInputs = netPiece.GetBytes(NUM_INPUTS);
	float weaponAngle = netPiece.GetFloat();
	//	netPiece.FeedStats();
	m_GameServer.SetInputs(clientId, pInputs, weaponAngle);
	m_Clients[clientId].SetInputs(pInputs);
}

byte_t Server::InGameClients()
{
	byte_t inGameClients = 0;
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (IsClientInGame(i))
			++inGameClients;
	}
	return inGameClients;
}

void Server::SendSnapshotToClient(int clientId) {

	// to be sure that outbound client go timeout
	//	if (m_NetServer.ClientOutbound(clientId))
	//		return;

	// update, delta and compress to get snapshot sendable
	//	m_SnapshotMgrs[clientId].GenerateAuraSnapshot(clientId);
	m_SnapshotMgrs[clientId].Update();

	// build the packaged message
	NetPiece netPiece;
	netPiece.AddByte(AUTH_COM);
	netPiece.AddByte(NET_SNAPSHOT);
	netPiece.Add(ServerSnapshotManager::GetLastDeltaSnapCompressed());

	// Monitoring things
	MonitoringVars::snapSize = netPiece.GetSize();
	netPiece.AddInt16(MonitoringVars::snapSize);
	if ( MonitoringVars::snapSizeMin > MonitoringVars::snapSize)
		MonitoringVars::snapSizeMin = MonitoringVars::snapSize;
	if ( MonitoringVars::snapSizeMax < MonitoringVars::snapSize)
		MonitoringVars::snapSizeMax = MonitoringVars::snapSize;
	netPiece.AddInt16(MonitoringVars::snapSizeMin);
	netPiece.AddInt16(MonitoringVars::snapSizeMax);

	unsigned int snapSequence = m_NetServer.SendSnapshotToClient(clientId, netPiece);

	m_SnapshotMgrs[clientId].SaveLastClientSnapshot(snapSequence);
}

void Server::SendSnapshotToClients() {
	if (AnyClientInGame()) {
		ServerSnapshotManager::GenerateSnapshot();
		for (int clientId = 0; clientId < MAX_CLIENTS; clientId++) {
			if (IsClientInGame(clientId) && m_NetServer.ReadyToSendSnapshot(clientId)) {
				SendSnapshotToClient(clientId);
			}
		}
	}
}


bool Server::Init()
{
	if (!InitializeSockets())
	{
		printf("failed to initialize sockets\n");
		return false;
	}

	if (!m_NetServer.initNetwork())
	{
		printf("could not init network on port %d\n", m_NetServer.getPort());
		return false;
	}

	return true;
}

void Server::Run()
{
	NetPiece::InitWordStats(100);

	Time::init();

	int lastNetUpdate = Time::ms();
	int lastSimUpdate = Time::ms();

	while (true)
	{
		// --- NETWORK ---

		if (Time::ms() - lastNetUpdate > 33) //TODO use regulator
		{
			lastNetUpdate = Time::ms();
			m_NetServer.CheckConnectionsStates();

			NetPiece netPiece;
			while (m_NetServer.Receive(netPiece))
			{
				ProcessPacket(netPiece);
				netPiece.Reset();
			}

			SendSnapshotToClients();
			SendEvents();

			m_NetServer.UpdateConnections();
		}

		// --- SIMULATION ---

		if (Time::ms() - lastSimUpdate > 16) //TODO use regulator
		{
			lastSimUpdate = Time::ms();

			if (m_NetServer.SomeNewClientConnected())
			{
				const bool* pConnected = m_NetServer.getConnected();

				// reset snapshot manager
				std::vector<Character*> &chars = m_GameServer.Characters();
				for (int i = 0; i < MAX_CLIENTS; ++i) {
					if ( pConnected[i] &&  chars[i]->GetState() == INACTIVE ) {
						m_SnapshotMgrs[i].Reset();
					}
				}

				m_GameServer.InitNewClients(pConnected);
				m_NetServer.NoNewClientConnected();
			}

			if (m_NetServer.SomeClientDisconnected())
			{
				const bool* pConnected = m_NetServer.getConnected();
				m_GameServer.DeactivateDisconnectedClients(pConnected);
				m_NetServer.NoClientDisconnected();
			}

			// Update the World
			m_GameServer.OnTick();

			++m_CurrentTick;
		}

		wait_second(0.001f);
	}
}

void Server::Shutdown()
{
	m_NetServer.Shutdown();
}

void Server::PrintDim(const char *name, float w, float h)
{
	printf("%s = ( %f m , %f m)\n", name, w, h);
}

void Server::SendMapInfo(int clientId)
{
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	netPiece.AddByte(NETMSG_MAPINFO);
	const char *mapName = m_GameServer.GetMap()->GetName();
	netPiece.AddString(mapName);
	m_NetServer.Send(netPiece, clientId);
}

void Server::HandleConnectionFail(int clientId, int failType)
{
	SendConnectionFail(clientId, failType);
	m_NetServer.Disconnect(clientId);
}

void Server::SendConnectionFail(int clientId, int failType)
{
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	netPiece.AddByte(NETMSG_CONNECTION_FAIL);
	netPiece.AddByte(failType);
	m_NetServer.Send(netPiece, clientId);
}

void Server::SendReady(int clientId)
{
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	byte_t msgType = NETMSG_READY;
	netPiece.AddByte(&msgType);

	// client must know his client id
	byte_t cid = clientId;
	netPiece.AddByte(&cid);

	byte_t inGameClients = InGameClients() + 1; // include the client connecting
	netPiece.AddByte(&inGameClients);

	// add clients info
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (m_Clients[i].IsInGame() || i == clientId) // include player connecting
		{
			netPiece.AddByte(i);
			netPiece.AddString(m_Clients[i].aName);
			netPiece.AddByte(m_Clients[i].skinId);
		}
	}

	m_NetServer.Send(netPiece, clientId);
}

void Server::SendChatAllClients(const char *msg) {
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	netPiece.AddByte(NETMSG_CHAT);
	netPiece.AddString(msg);
	SendMsgAllClients(netPiece);
}

void Server::SendClientEnterGame(int clientId)
{
	SendClientDataChange(clientId);
	char msg[200];
	std::sprintf(msg, "%s joined the game", m_Clients[clientId].aName);
	SendChatAllClients(msg);
}

void Server::SendClientLeaveGame(int clientId)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (IsClientInGame(i))
		{
			char msg[200];
			std::sprintf(msg, "%s leave the game", m_Clients[clientId].aName);
			SendChat(i, msg);
		}
	}
}

void Server::SendChat(int clientId, const char *pMsg) {
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	byte_t msgType = NETMSG_CHAT;
	netPiece.AddByte(&msgType);
	netPiece.AddString(pMsg);
	m_NetServer.Send(netPiece, clientId);
}

void Server::SendClientDataChange(int clientId)
{
	NetPiece netPiece(NO_AUTH_COM, NET_MSG);
	netPiece.AddByte(NETMSG_CLIENT_DATA_CHANGE);
	netPiece.AddByte(clientId);
	netPiece.AddString(m_Clients[clientId].aName);
	netPiece.AddByte(m_Clients[clientId].skinId);
	SendMsgAllClients(netPiece);
}

void Server::SendMsgAllClients(NetPiece& netPiece)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
		if (IsClientInGame(i))
			m_NetServer.Send(netPiece, i);
}

bool Server::AnyClientInGame() const {
	for (int i = 0; i < MAX_CLIENTS; ++i)
		if (IsClientInGame(i))
			return true;
	return false;
}

void Server::SendEvents() {
	if (GameServer::Event::numEvents > 0) {
		NetPiece np(NO_AUTH_COM, NET_MSG);
		np.AddByte(NETMSG_EVENTS);
		m_GameServer.GetEvents(np);
		SendMsgAllClients(np);
		GameServer::Event::numEvents = 0;
	}
}
