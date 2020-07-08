#ifndef NETSERVER_H_
#define NETSERVER_H_

#include <engine/shared/Socket.h>
#include <engine/shared/ReliableConnection.h>
#include <engine/shared/FlowControl.h>
#include <engine/shared/NetPiece.h>

class NetServer
{
	public:
		enum
		{
			NUM_INBOUND_SLOTS = 8,
			NUM_OUTBOUND_SLOTS = 3,
			NUM_SLOTS = NUM_INBOUND_SLOTS + NUM_OUTBOUND_SLOTS
		};

	private:
		static const int m_Port = 30000;
		static const int m_ProtocolId = 0x11223344;
		static constexpr float m_DeltaTime = 1.0f / 30.0f;
		static constexpr float m_SendRate = 1.0f / 30.0f;
		static constexpr float m_TimeOut = 10.0f;

		float m_SendAccumulators[NUM_SLOTS];
		float m_StatsAccumulators[NUM_SLOTS];
		Socket m_Socket;
		std::vector<ReliableConnection> m_Connections;
		bool m_Connected[NUM_SLOTS];

		std::vector<FlowControl> m_FlowControls;

		// test things
		int m_TimeInSeconds;
		char m_CharTestData;

		bool m_NewInboundClientConnected;
		bool m_InboundClientDisconnected;

	public:

		NetServer();

		const int getPort() const
		{
			return m_Port;
		}
		const int getProtocolId() const
		{
			return m_ProtocolId;
		}
		const float getDeltaTime() const
		{
			return m_DeltaTime;
		}
		const float getSendRate() const
		{
			return m_SendRate;
		}
		const float getTimeOut() const
		{
			return m_TimeOut;
		}

		bool initNetwork()
		{
			printf("start connection on port %d\n", m_Port);
			if ( !m_Socket.Open(m_Port) )
				return false;
			return true;
		}

		void Shutdown();
		void CheckConnectionsStates();
		bool Receive(NetPiece &netPiece);
		bool ReadyToSendSnapshot(int clientId);
		unsigned int SendSnapshotToClient(int clientId, const NetPiece &netPiece);
		void UpdateConnections();
		void ShowConnectionStats();
		bool isValidProtocol(const unsigned char* packet);
		int getClientId(const Address &sender);

		bool SomeNewClientConnected() const
		{
			return m_NewInboundClientConnected;
		}

		void NoNewClientConnected()
		{
			m_NewInboundClientConnected = false;
		}

		const bool* getConnected() const
		{
			return m_Connected;
		}

		bool SomeClientDisconnected() const
		{
			return m_InboundClientDisconnected;
		}

		void NoClientDisconnected()
		{
			m_InboundClientDisconnected = false;
		}

		void Send(NetPiece &netPiece, int clientId);

		void Disconnect(int clientId) {
			m_Connections[clientId].OnDisconnect();
		}

		const std::list<unsigned int> & GetAckedSequences(int clientId) {
			return m_Connections[clientId].GetAckedSequences();
		}

		bool ClientInbound(int clientId) const {
			return clientId >= 0 && clientId < NUM_INBOUND_SLOTS;
		}
};

#endif /* NETSERVER_H_ */
