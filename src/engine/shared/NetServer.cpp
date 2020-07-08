#include "NetServer.h"
#include <engine/shared/Net.h>
#include <engine/shared/Common.h>
#include <base/tri_logger.hpp>

NetServer::NetServer() :
m_Socket(), m_Connections(NUM_SLOTS, ReliableConnection(m_Socket, m_ProtocolId, m_TimeOut)), m_FlowControls(NUM_SLOTS), m_TimeInSeconds(
		0), m_CharTestData(0), m_NewInboundClientConnected(false), m_InboundClientDisconnected(false)
{

	for(int i=0; i<NUM_SLOTS; i++) {
		m_SendAccumulators[i] = 0;
		m_StatsAccumulators[i] = 0;
	}
	std::memset(&m_Connected, 0, NUM_SLOTS);
}

void NetServer::CheckConnectionsStates()
{
	for (int i = 0; i < NUM_SLOTS; i++)
	{
		// update flow control
		if ( m_Connections[i].IsConnected() )
		{
			m_FlowControls[i].Update(m_DeltaTime, m_Connections[i].GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
		}

		// detect changes in connection state
		if ( m_Connected[i] && !m_Connections[i].IsConnected() )
		{
			m_FlowControls[i].Reset();
			m_Connected[i] = false;

			if (ClientInbound(i))
				m_InboundClientDisconnected = true;
		}

		if ( !m_Connected[i] && m_Connections[i].IsConnected() )
		{
			m_Connected[i] = true;
		}

		if ( !m_Connected[i] && m_Connections[i].ConnectFailed() )
		{
			printf("connection failed\n");
			break;
		}
	}
}

bool NetServer::Receive(NetPiece &netPiece)
{
	const int headerSize = 16;
	const int dataSize = MAX_PACKET_SIZE;
	unsigned char packet[headerSize + dataSize];
	Address sender;

	int recv_bytes = m_Socket.Receive(sender, packet, dataSize + headerSize);

	// no more message from client to process
	if ( recv_bytes <= 0 || recv_bytes <= headerSize )
		return false;

	if ( !isValidProtocol(packet) )
	{
		printf("Not valid protocol !!!");
		return true;
	}

	unsigned int packet_sequence = 0;
	unsigned int packet_ack = 0;
	unsigned int packet_ack_bits = 0;
	ReliableConnection::ReadHeader(&packet[4], packet_sequence, packet_ack, packet_ack_bits);

	int clientId = getClientId(sender);

	// new client
	if ( clientId == INVALID_CLIENT_ID )
	{
		// searching for a free slot
		for (int currentClientId = 0; currentClientId < NUM_SLOTS; currentClientId++)
		{
			if ( m_Connections[currentClientId].IsFree() )
			{
				clientId = currentClientId;
				m_Connections[currentClientId].ClearData();
				m_Connections[currentClientId].Init(sender);
				m_Connections[currentClientId].UpdateReliabilitySysOnRecv(packet_sequence, packet_ack, packet_ack_bits, recv_bytes - headerSize);
				m_Connected[currentClientId] = true;

				if (ClientInbound(clientId))
					m_NewInboundClientConnected = true;

				netPiece.SetAsFirstPacket(); // very first packet
				break;
			}
		}
	}
	// le client est connu
	else
	{
		m_Connections[clientId].UpdateReliabilitySysOnRecv(packet_sequence, packet_ack, packet_ack_bits, recv_bytes - headerSize);
	}


	if ( clientId != INVALID_CLIENT_ID )
	{
		// need to know from what client that packet comes from
		netPiece.SetClientId(clientId);

		m_Connections[clientId].ResetTimeout();

		// add data to packet
		unsigned char *dataPacket = &packet[headerSize];
		netPiece.AddBytes(dataPacket, MAX_PACKET_SIZE);
	}

	return true;
}

unsigned int NetServer::SendSnapshotToClient(int clientId, const NetPiece &netPiece)
{
	assert(clientId >= 0 && clientId < NUM_INBOUND_SLOTS);

	const float sendRate = m_FlowControls[clientId].GetSendRate();
	m_SendAccumulators[clientId] += m_DeltaTime;
	unsigned int sentSequence = 0;
	const unsigned char *packet = netPiece.GetData();
	int size = netPiece.GetSize();
	m_Connections[clientId].SendPacket(packet, size, &sentSequence);
	m_SendAccumulators[clientId] -= 1.0f / sendRate;

	return sentSequence;
}


bool NetServer::ReadyToSendSnapshot(int clientId)
{
	assert(clientId >= 0 && clientId < NUM_INBOUND_SLOTS);

	const float sendRate = m_FlowControls[clientId].GetSendRate();
	m_SendAccumulators[clientId] += m_DeltaTime;
	return m_Connections[clientId].IsConnected() && (m_SendAccumulators[clientId] > 1.0f / sendRate);
}

void NetServer::UpdateConnections()
{
	for (int i = 0; i < NUM_SLOTS; i++)
	{
		//TODO update for other state than connected
		if ( m_Connections[i].IsConnected() )
		{
			m_Connections[i].Update(m_DeltaTime);
		}
	}
}

bool NetServer::isValidProtocol(const unsigned char* packet)
{
	if ( packet[0] != (unsigned char) (m_ProtocolId >> 24) || packet[1] != (unsigned char) ((m_ProtocolId >> 16) & 0xFF)
			|| packet[2] != (unsigned char) ((m_ProtocolId >> 8) & 0xFF) || packet[3] != (unsigned char) (m_ProtocolId & 0xFF) )
		return false;
	return true;
}

int NetServer::getClientId(const Address &sender)
{
	for (int i = 0; i < NUM_SLOTS; i++)
	{
		if ( sender == m_Connections[i].GetAddress() )
		{
			return i;
		}
	}

	return INVALID_CLIENT_ID;
}

void NetServer::ShowConnectionStats()
{
	for (int i = 0; i < NUM_SLOTS; i++)
	{
		if ( !m_Connections[i].IsConnected() )
			continue;

		m_StatsAccumulators[i] += m_DeltaTime;

		while (m_StatsAccumulators[i] >= 0.25f && m_Connections[i].IsConnected())
		{
			float rtt = m_Connections[i].GetReliabilitySystem().GetRoundTripTime();

			unsigned int sent_packets = m_Connections[i].GetReliabilitySystem().GetSentPackets();
			unsigned int acked_packets = m_Connections[i].GetReliabilitySystem().GetAckedPackets();
			unsigned int lost_packets = m_Connections[i].GetReliabilitySystem().GetLostPackets();

			float sent_bandwidth = m_Connections[i].GetReliabilitySystem().GetSentBandwidth();
			float acked_bandwidth = m_Connections[i].GetReliabilitySystem().GetAckedBandwidth();

			printf("[Client %d]  rtt %.1fms, sent %d, acked %d, lost %d (%.1f%%), sent bandwidth = %.1fkbps, acked bandwidth = %.1fkbps\n",
					i, rtt * 1000.0f, sent_packets, acked_packets, lost_packets,
					sent_packets > 0.0f ? (float) lost_packets / (float) sent_packets * 100.0f : 0.0f, sent_bandwidth, acked_bandwidth);

			m_StatsAccumulators[i] -= 0.25f;
		}
	}
}

void NetServer::Shutdown()
{
	ShutdownSockets();
}

void NetServer::Send(NetPiece &netPiece, int clientId)
{
	assert(clientId >= 0 && clientId < NUM_SLOTS);
	const unsigned char *packet = netPiece.GetData();
	int size = netPiece.GetSize();
	assert(size > 0 && size <= MAX_PACKET_SIZE);
	m_Connections[clientId].SendPacket(packet, size);
}


