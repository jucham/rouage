#include "Pistol.h"
#include "WeaponSystem.h"
#include "Rocket.h"
#include "GameServer.h"
#include "EntityEnumerations.h"
#include "Tile.h"

Pistol::Pistol(WeaponSystem* pws) :
      Weapon(pws, WP_PISTOL)
{
   assert(pws != 0);

   m_DelayBetweenShots = 25;
   m_AmmoCount = 10;
   m_MaxAmmoCount = 10;
}

void Pistol::Fire() const
{
	m_pWeaponSys->GetGameServer()->AddProjectile(m_pWeaponSys->Shooter(), m_pWeaponSys->Facing(), WP_PISTOL);
}
