//TODO COMMENT
#ifndef POWERUPGIVER_H
#define POWERUPGIVER_H

#include "TriggerRespawning.h"

class PowerupGiver: public TriggerRespawning
{
   public:
      PowerupGiver(GameServer *pGameServer, TriggerSystem* pTrigSys, Vector2D pos, int powerupType);
      int GetSubType() const { return m_iPowerupType; }

   private:
      int m_iPowerupType;
};

#endif // POWERUPGIVER_H
