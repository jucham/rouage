#ifndef TRIGGERRESPAWNING_H
#define TRIGGERRESPAWNING_H

#include "Trigger.h"
class GameServer;

class TriggerRespawning : public Trigger
{
   public:
      TriggerRespawning(TriggerSystem* pTrigSys, GameServer *pGameServer);
      virtual ~TriggerRespawning();

      void Desactivate();
      void Update();

      bool MustRespawn();

   protected:
      GameServer *m_pGameServer;
      int m_RespawnDelay;
      int m_TimeNextRespawn;


   private:
};

#endif // TRIGGERRESPAWNING_H
