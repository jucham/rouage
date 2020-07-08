#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#include <base/DetectPlatform.h>
#include <engine/shared/FlowControl.h>
#include <engine/shared/ReliableConnection.h>
#include <game/client/GameClient.h>
#include <engine/shared/Snapshot.h>
#include <engine/shared/Net.h>
#include <engine/shared/NetPiece.h>
#include <engine/shared/Huffman.h>
#include "NetClient.h"
#include "InputManager.h"
#include "CommandManager.h"
#include <game/client/ui/Menu.h>
#include <engine/client/ClientSnapshotManager.h>


class Client
{
	friend class MenuHandler; // TODO friend with only one fonction
	friend class MenuEventHandler;

public:

	struct PlayerData {
		char name[MAX_PLAYERNAME_LENGTH];
		int skinId;
	};


	Client(const Address &address, bool dumb);
	bool InitNetwork();
	void Run();

	const Huffman & GetHuffman() const {return m_Huffman;}

private:

	/* Constants: Client States
		 STATE_OFFLINE - The client is offline.
		 STATE_CONNECTING - The client is trying to connect to a server.
		 STATE_LOADING - The client has connected to a server and is loading resources.
		 STATE_ONLINE - The client is connected to a server and running the game.
		 STATE_QUITING - The client is quiting.
	 */
	enum
	{
		STATE_OFFLINE = 0, STATE_CONNECTING, STATE_LOADING, STATE_ONLINE, STATE_QUITING
	};

	bool       m_ConnectionInitialized;
	bool       m_ConnectionFail;
	int        m_ConnectionFailType;
	int        m_State;
	NetClient  m_NetClient;
	Address    m_ServerAddress;
	GameClient m_GameClient;
	int        m_LocalClientId;
	float      m_SnapRateAverage;
	float      m_SnapLatencyAverage;
	bool       m_ReadyToExit;
	bool       m_ExitRequested;
	bool       m_LeaveGameRequested;
	PlayerData m_LocalPlayerData;
	PlayerData m_PlayerData[MAX_CLIENTS];
	Huffman    m_Huffman;
	bool       m_InMenu;
	MenuStack  m_MenuStack;
	ClientSnapshotManager m_SnapshotMgr;

	bool m_Dumb;


	bool InitConnection(const Address &address);
	bool IsRunning() const;
	void UpdateExitRequest();
	void Send();
	void SendProtocolVersion();
	void SendReady();
	void SendEnterInGame();
	void SendInputs();
	void SendLeave();
	void SendChat(const char *pMsg);
	void ProcessPacket(NetPiece &netPiece);
	void DebugShowAcks();
	void ProcessMessage(NetPiece& netPiece);
	void ProcessSnapshot(NetPiece& netPiece);
//	void GetUncompressedSnapshot(NetDataArray<> &nda, NetPiece &np) const;
	void ProcessCommands();
	bool JoinGame(const CEGUI::EventArgs& ea);
	bool ExitGame(const CEGUI::EventArgs& ea);
	void LeaveGame();
	void InjectTimePulse() const;

};

inline bool Client::IsRunning() const
{
	return !m_ReadyToExit;
}

inline void Client::UpdateExitRequest() {
	if (!m_ExitRequested)
		m_ExitRequested = InputManager::Instance().IsExitInputTriggered();
}

#endif
