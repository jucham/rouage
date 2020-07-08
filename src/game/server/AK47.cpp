#include "AK47.h"
#include "WeaponSystem.h"
#include "Rocket.h"
#include <game/server/GameServer.h>
#include "EntityEnumerations.h"
#include "Tile.h"


AK47::AK47(WeaponSystem* pws) :
   Weapon(pws,
          WP_AK47)
{
   assert ( pws != 0 );

   m_DelayBetweenShots = 100;
   m_AmmoCount = 50;
   m_MaxAmmoCount = 50;
}

void AK47::Fire() const
{
   m_pWeaponSys->GetGameServer()->AddProjectile( m_pWeaponSys->Shooter(), m_pWeaponSys->Facing(), WP_AK47);
}

