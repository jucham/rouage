#ifndef WEAPONGIVER_H
#define WEAPONGIVER_H

#include "TriggerRespawning.h"
#include "WeaponSystem.h"

class WeaponGiver : public TriggerRespawning
{
   public:
      WeaponGiver(GameServer *pGameServer, TriggerSystem* pTrigSys, int weaponType, Vector2D pos);
      void DoAction();

      int GetSubType() const { return m_iWeaponType; }

   private:

    int m_iWeaponType;

};

#endif // WEAPONGIVER_H
