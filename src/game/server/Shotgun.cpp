#include "Shotgun.h"
#include "WeaponSystem.h"
#include "Rocket.h"
#include <game/server/GameServer.h>
#include "EntityEnumerations.h"
#include "Tile.h"

Shotgun::Shotgun(WeaponSystem* pws) :
   Weapon(pws,
          WP_SHOTGUN)
{
   assert ( pws != 0 );

   m_DelayBetweenShots = 500;
   m_AmmoCount = 100;
   m_MaxAmmoCount = 10;
   m_fHandOffset = Character::m_fARM_WIDTH * 1.1f;
   m_fProjCannonOffset = m_fHeight/2 * 0.33f;
}

void Shotgun::Fire() const
{
   // the centered projectile
   m_pWeaponSys->GetGameServer()->AddProjectile( m_pWeaponSys->Shooter(), m_pWeaponSys->Facing(), WP_SHOTGUN);

   // the other projectiles
   float wpAngle = m_pWeaponSys->WeaponAngle();
   float angleBetweenProj = QuarterPi/16;
   for(int i=1; i<=2; i++)
   {
         Vector2D f1 ( std::cos( wpAngle + i*angleBetweenProj),  std::sin( wpAngle + i*angleBetweenProj) );
         m_pWeaponSys->GetGameServer()->AddProjectile( m_pWeaponSys->Shooter(), f1, WP_SHOTGUN);

         Vector2D f2 ( std::cos( wpAngle - i*angleBetweenProj),  std::sin( wpAngle - i*angleBetweenProj) );
         m_pWeaponSys->GetGameServer()->AddProjectile( m_pWeaponSys->Shooter(), f2, WP_SHOTGUN);
   }

   GameServer::Event event;
   event.type = EVENT_FIRE_SHOTGUN;
   event.pos = m_pWeaponSys->WeaponPos();
   event.ownerClientId = m_pWeaponSys->Shooter()->GetClientId();
   m_pWeaponSys->GetGameServer()->AddEvent(event);
}

