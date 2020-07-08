#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <base/DetectPlatform.h>
#include <base/CircularBuffer.h>
#include <engine/shared/NetServer.h>
#include <game/server/GameServer.h>
#include <engine/shared/Net.h>
#include <engine/shared/Common.h>
#include <engine/shared/NetPiece.h>
#include <engine/server/ServerSnapshotManager.h>

#include <cstdio>

class Server
{
	friend class ServerSnapshotManager;

public:

	static FILE *logfile;

	struct Inputs
	{
		int m_aData[NUM_INPUTS]; //TODO taille a d�finir et type int ???

		Inputs()
		{
			Init();
		}

		void Init()
		{
			std::memset(m_aData, 0, NUM_INPUTS);
		}
	};

	/*
	 * Client is useful to store info about client (state, inputs, etc.)
	 */
	struct Client
	{

		enum
		{
			STATE_EMPTY, STATE_AUTH, STATE_CONNECTING, STATE_READY, STATE_INGAME
		};

		Client() :
			state(STATE_EMPTY), inputs()
		{
		}

		char aName[MAX_PLAYERNAME_LENGTH];
		int skinId;
		int state;
		Inputs inputs;

		void Init()
		{
			std::memset(aName, 0, MAX_PLAYERNAME_LENGTH);
			state = STATE_AUTH;
			inputs.Init();
		}

		bool IsAuth() const
			{
			return state == STATE_AUTH;
		}

		bool IsConnecting() const
		{
			return state == STATE_CONNECTING;
		}

		bool IsReady() const
		{
			return state == STATE_READY;
		}

		bool IsInGame() const
		{
			return state == STATE_INGAME;
		}

		void SetStateAsConnecting()
		{
			state = STATE_CONNECTING;
		}
		void SetStateAsReady()
		{
			state = STATE_READY;
		}
		void SetStateAsInGame()
		{
			state = STATE_INGAME;
		}

		void SetInputs(const unsigned char *pInputs)
		{
			std::memcpy(inputs.m_aData, pInputs, NUM_INPUTS);
		}
	};

	Server();
	bool Init();
	void Run();
	void Shutdown();
	inline bool IsClientInGame(int clientId) const;
	inline const char * GetClientName(int clientId) const;

private:
	std::vector<Client> m_Clients;
	NetServer m_NetServer;
	GameServer m_GameServer;
	int m_CurrentTick;
	ServerSnapshotManager m_SnapshotMgrs[MAX_CLIENTS];

	inline bool GetClientState(int clientId) const;
	inline void SetClientState(int clientId, int state);
	void SendConnectionFail(int clientId, int failType);
	void HandleConnectionFail(int clientId, int failType);
	void SendMapInfo(int clientId);
	void SendReady(int clientId);
	void SendClientEnterGame(int clientId);
	void SendClientLeaveGame(int clientId);
	void SendChat(int clientId, const char *pMsg);
	void SendChatAllClients(const char *msg);
	void SendClientDataChange(int clientId);
	void SendMsgAllClients(NetPiece& netPiece);
	void SendEvents();
	static void PrintDim(const char *name, float w, float h);
	void ProcessPacket(NetPiece &netPiece);
	void SendSnapshotToClients();
	void SendSnapshotToClient(int clientId);
	byte_t InGameClients();
	void ProcessMessage(int clientId, NetPiece& netPiece);
	void ProcessInputs(int clientId, NetPiece& netPiece);
	bool AnyClientInGame() const;
	inline const GameServer & GetGameServer() const;

};

inline bool Server::GetClientState(int clientId) const
{
	assert(clientId >= 0 && clientId < MAX_CLIENTS);
	return m_Clients[clientId].state;
}

inline void Server::SetClientState(int clientId, int state)
{
	assert(clientId >= 0 && clientId < MAX_CLIENTS);
	assert(state >= Client::STATE_EMPTY && state <= Client::STATE_INGAME);
	m_Clients[clientId].state = state;
}

inline bool Server::IsClientInGame(int clientId) const
{
	assert(clientId >= 0 && clientId < MAX_CLIENTS);
	return m_Clients[clientId].IsInGame();
}


inline const char * Server::GetClientName(int clientId) const
{
	assert(clientId >= 0 && clientId < MAX_CLIENTS);
	return m_Clients[clientId].aName;
}

const GameServer & Server::GetGameServer() const {return m_GameServer;}
