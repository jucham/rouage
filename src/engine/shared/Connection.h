/*
 * Connection.h
 *
 *  Created on: 14 mai 2012
 *      Author: julien
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <cassert>
#include <cstdio>
#include <cstring>
#include "Address.h"
#include "Socket.h"

class Connection
{
public:

	Connection(Socket& socket, unsigned int protocolId, float timeout) :
		m_ProtocolId(protocolId),
		m_Timeout(timeout),
		m_Running(false),
		m_State(STATE_DISCONNECTED),
		m_Socket(socket),
		timeoutAccumulator(0.0f),
		m_Address()
{}

	virtual ~Connection()
	{
		if (IsRunning()) Stop();
	}

	bool Start(int port)
	{
		assert(!m_Running);
		printf("start connection on port %d\n", port);
		if (!m_Socket.Open(port)) return false;
		m_Running = true;
		OnStart();
		return true;
	}

	void Stop()
	{
		//assert(m_Running);
		printf("stop connection\n");
		bool connected = IsConnected();
		ClearData();
		m_Socket.Close();
		m_Running = false;
		if (connected) OnDisconnect();
		OnStop();
	}

	bool IsRunning() const
	{
		return m_Running;
	}

	void Listen()
	{
		printf("server listening for connection\n");
		bool connected = IsConnected();
		ClearData();
		if (connected) OnDisconnect();
		m_State = STATE_LISTENING;
	}

	void Connect(const Address & address)
	{
		printf("client connecting to %d.%d.%d.%d:%d\n", address.GetA(), address.GetB(), address.GetC(), address.GetD(), address.GetPort());
		bool connected = IsConnected();
		ClearData();
		if (connected) OnDisconnect();
		m_State = STATE_CONNECTING;
		this->m_Address = address;
	}

	bool IsFree() const
	{
		return m_State == STATE_DISCONNECTED;
	}

	bool IsConnecting() const
	{
		return m_State == STATE_CONNECTING;
	}

	bool ConnectFailed() const
	{
		return m_State == STATE_CONNECTFAIL;
	}

	bool IsConnected() const
	{
		return m_State == STATE_CONNECTED;
	}

	bool IsListening() const
	{
		return m_State == STATE_LISTENING;
	}

	virtual void Update(float deltaTime)
	{
		//assert(m_Running);

		timeoutAccumulator += deltaTime;
		if (timeoutAccumulator > m_Timeout)
		{
			if (m_State == STATE_CONNECTING)
			{
				printf("connect timed out\n");
				ClearData(); // FIXME cleardata called twice (see onDisconnected)
				m_State = STATE_CONNECTFAIL;
				OnDisconnect();
			}
			else if (m_State == STATE_CONNECTED)
			{
				printf("connection timed out\n");
				ClearData();
				m_State = STATE_DISCONNECTED;
				OnDisconnect(); // FIXME cleardata called twice (see onDisconnected)
			}
		}
	}

	virtual bool SendPacket(const unsigned char data[], int size);

	virtual int ReceivePacket(unsigned char data[], int size)
	{
		//         assert (running);
		unsigned char packet[size + 4];
		Address sender;
		int bytes_read = m_Socket.Receive(sender, packet, size + 4);
		if (bytes_read == 0) return 0;
		if (bytes_read <= 4) return 0;
		if (packet[0] != (unsigned char) (m_ProtocolId >> 24) || packet[1] != (unsigned char) ((m_ProtocolId >> 16) & 0xFF)
				|| packet[2] != (unsigned char) ((m_ProtocolId >> 8) & 0xFF) || packet[3] != (unsigned char) (m_ProtocolId & 0xFF)) return 0;

		if (sender == m_Address)
		{
			timeoutAccumulator = 0.0f;
			memcpy(data, &packet[4], bytes_read - 4);
			return bytes_read - 4;
		}
		return 0;
	}

	int GetHeaderSize() const
	{
		return 4;
	}

	const Address & GetAddress() const
	{
		return m_Address;
	}

	void SetAddress(const Address &address)
	{
		m_Address = address;
	}

	void Init(const Address &address)
	{
		printf("Server accepts connection from client ");
		address.print();
		m_State = STATE_CONNECTED;
		m_Address = address;
	}

	void SetConnected()
	{
		m_State = STATE_CONNECTED;
	}

	void ResetTimeout()
	{
		timeoutAccumulator = 0.0f;
	}

protected:

	virtual void OnStart()
	{
	}
	virtual void OnStop()
	{
	}
	virtual void OnConnect()
	{
	}
	virtual void OnDisconnect()
	{
		ClearData();
	}

protected:

	void ClearData()
	{
		m_State = STATE_DISCONNECTED;
		timeoutAccumulator = 0.0f;
		m_Address = Address();
	}

	enum State
	{
		STATE_DISCONNECTED,
		STATE_LISTENING,
		STATE_CONNECTING,
		STATE_CONNECTFAIL,
		STATE_CONNECTED
	};

	unsigned int m_ProtocolId;
	float m_Timeout;
	bool m_Running;
	State m_State;
	Socket& m_Socket;
	float timeoutAccumulator;
	Address m_Address;

};

inline bool Connection::SendPacket(const unsigned char data[], int size)
{
	if (m_Address.GetAddress() == 0)
		return false;

	unsigned char packet[size + 4];
	packet[0] = (unsigned char) ((m_ProtocolId >> 24));
	packet[1] = (unsigned char) (((m_ProtocolId >> 16) & 0xFF));
	packet[2] = (unsigned char) (((m_ProtocolId >> 8) & 0xFF));
	packet[3] = (unsigned char) (((m_ProtocolId) & 0xFF));
	memcpy(&packet[4], data, size);

	return m_Socket.Send(m_Address, packet, size + 4);
}

#endif /* CONNECTION_H_ */
