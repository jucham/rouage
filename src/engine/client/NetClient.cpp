#include "NetClient.h"
#include <base/utils.h>
#include <engine/shared/Net.h>
#include <engine/shared/Common.h>
#include <engine/shared/NetPiece.h>


int NetClient::m_ClientPort = 30001;

NetClient::NetClient() :
m_Socket(), m_FlowControl(), m_Connection(m_Socket, m_ProtocolId, 10.0f), // m_TimeOut),
m_Connected(false), m_SendRate(0.0f), m_SendAccumulator(0.0f), m_StatsAccumulator(0.0f)
{}

void NetClient::Reset() {
	m_Socket.Reset();
	m_FlowControl.Reset();
	m_Connection.ClearData();
	m_Connected = false;
	m_SendRate = 0.0f;
	m_SendAccumulator = 0.0f;
	m_StatsAccumulator = 0.0f;
}

bool NetClient::StartConnection(const Address &address)
{
	Reset();

	bool success = false;
	int numPortTests = MAX_CLIENTS+1;
	while (numPortTests--) {
		if ( m_Connection.Start(m_ClientPort) ) {
			success = true;
			break;
		} else {
			printf("could not start connection on port %d\n", m_ServerPort);
			m_ClientPort += 1;
		}
	}

	if (!success) {
		return false;
	}

	m_Connection.Connect(address);
	return true;
}

void NetClient::StopConnection() {
	if (m_Connection.IsRunning())
		m_Connection.Stop();
}

void NetClient::UpdateFlowControl()
{
	// update flow control
	if ( m_Connection.IsConnected() )
	{
		m_FlowControl.Update(m_DeltaTime, m_Connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
	}
	m_SendRate = m_FlowControl.GetSendRate();
}

bool NetClient::CheckConnectionState()
{
	// detect changes in connection state
	if ( !m_Connected && m_Connection.IsConnected() )
	{
		printf("client connected to server\n");
		m_Connected = true;
	}

	if ( !m_Connected && m_Connection.ConnectFailed() )
	{
		printf("connection failed\n");
		return false;
	}

	return true;
}

bool NetClient::Send(const NetPiece& netPiece)
{
	const unsigned char *packet = netPiece.GetData();
	int size = netPiece.GetSize();

	// data are not input we can send immediately
	if ( !netPiece.isInput() )
	{
		m_Connection.SendPacket(packet, size);
		return true;
	}
	// data are inputs, we need to send them at regular rate
	else
	{
		m_SendAccumulator += m_DeltaTime; 
		if (m_SendAccumulator > 1.0f / m_SendRate)
		{
			m_Connection.SendPacket(packet, size);
			m_SendAccumulator -= 1.0f / m_SendRate;
			return true;
		}
	}

	return false;
}

bool NetClient::Receive(NetPiece &netPiece)
{
	unsigned char packet[MAX_PACKET_SIZE];
	int bytesRead = m_Connection.ReceivePacket(packet, sizeof(packet));

	if ( bytesRead == 0 )
		return false;

	if ( m_Connection.IsConnecting() )
	{
		printf("client completes connection with server\n");
		m_Connection.SetConnected();
	}

	netPiece.AddBytes(packet, MAX_PACKET_SIZE);
	return true;
}

void NetClient::UpdateConnection()
{
	m_Connection.Update(m_DeltaTime);
}

void NetClient::DebugShowAcks()
{
	unsigned int * acks = NULL;
	int ack_count = 0;
	m_Connection.GetReliabilitySystem().GetAcks(&acks, ack_count);
	if ( ack_count > 0 )
	{
		printf("acks: %d", acks[0]);
		for (int i = 1; i < ack_count; ++i)
			printf(",%d", acks[i]);
		printf("\n");
	}
}

void NetClient::ShowConnectionStats()
{
	// show connection stats
	m_StatsAccumulator += m_DeltaTime;

	while (m_StatsAccumulator >= 0.25f && m_Connection.IsConnected())
	{
		float rtt = m_Connection.GetReliabilitySystem().GetRoundTripTime();

		unsigned int sent_packets = m_Connection.GetReliabilitySystem().GetSentPackets();
		unsigned int acked_packets = m_Connection.GetReliabilitySystem().GetAckedPackets();
		unsigned int lost_packets = m_Connection.GetReliabilitySystem().GetLostPackets();

		float sent_bandwidth = m_Connection.GetReliabilitySystem().GetSentBandwidth();
		float acked_bandwidth = m_Connection.GetReliabilitySystem().GetAckedBandwidth();

		printf("rtt %.1fms, sent %d, acked %d, lost %d (%.1f%%), sent bandwidth = %.1fkbps, acked bandwidth = %.1fkbps\n", rtt * 1000.0f,
				sent_packets, acked_packets, lost_packets,
				sent_packets > 0.0f ? (float) lost_packets / (float) sent_packets * 100.0f : 0.0f, sent_bandwidth, acked_bandwidth);

		m_StatsAccumulator -= 0.25f;
	}
}

