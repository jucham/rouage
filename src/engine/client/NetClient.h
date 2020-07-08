#ifndef NETCLIENT_H_
#define NETCLIENT_H_

#include <engine/shared/Socket.h>
#include <engine/shared/ReliableConnection.h>
#include <engine/shared/FlowControl.h>

class NetPiece;

class NetClient
{
   public:
      NetClient();
      void Reset();
      void UpdateFlowControl();
      bool CheckConnectionState();
      bool Send(const NetPiece &netPiece);
      bool Receive(NetPiece &netPiece);
      void UpdateConnection();
      void DebugShowAcks();
      void ShowConnectionStats();

      bool StartConnection(const Address &address);
      void StopConnection();

      const int getServerPort() const
      {
         return m_ServerPort;
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

   protected:
      static const int   m_ServerPort = 30000;
      static int         m_ClientPort;
      static const int   m_ProtocolId = 0x11223344;
      static constexpr float m_DeltaTime = 1.0f / 30.0f;
      static constexpr	 float m_TimeOut = 10.0f;
      Socket             m_Socket;
      FlowControl        m_FlowControl;
      ReliableConnection m_Connection;
      bool               m_Connected;
      float              m_SendRate;
      float              m_SendAccumulator;
      float              m_StatsAccumulator;
};

#endif /* NETCLIENT_H_ */
