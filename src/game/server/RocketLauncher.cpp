#include "RocketLauncher.h"
#include "WeaponSystem.h"
#include <game/server/GameServer.h>
#include "EntityEnumerations.h"

RocketLauncher::RocketLauncher(WeaponSystem* pws) :
   Weapon(pws,
          WP_ROCKET_LAUNCHER)
{
   assert ( pws != 0 );

   m_DelayBetweenShots = 450;
   m_AmmoCount = 10;
   m_MaxAmmoCount = 10;
   m_fHandOffset = Character::m_fARM_WIDTH * 0.75f;
   m_fProjCannonOffset = m_fHeight *0.09f;
}

void RocketLauncher::Fire() const
{
   m_pWeaponSys->GetGameServer()->AddProjectile( m_pWeaponSys->Shooter(), m_pWeaponSys->Facing(), WP_ROCKET_LAUNCHER);
   //TODO add event explosion
}

