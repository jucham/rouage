#include "WeaponGiver.h"
#include "TriggerSystem.h"
#include <game/server/GameServer.h>
#include "Weapon.h"
#include "RocketLauncher.h"
#include "EntityEnumerations.h"
#include "Character.h"
#include "Tile.h"
#include "Map.h"


WeaponGiver::WeaponGiver(GameServer *pGameServer, TriggerSystem* pTrigSys, int weaponType, Vector2D pos) :
TriggerRespawning(pTrigSys, pGameServer),
m_iWeaponType(weaponType)
{
	m_Type = TG_ITEM_WEAPON;

	m_vPosition = pos;
	m_HitBox.setWidth(Tile::m_fWIDTH);
	m_HitBox.setHeight( 0.0f );
	m_HitBox.setPosition(pos);
}

void WeaponGiver::DoAction()
{
   // character doesn't own weapon
   if ( ! m_pCharTriggered->WeaponSys()->PossessWeapon(m_iWeaponType))
   {
      // get weapon
      m_pCharTriggered->WeaponSys()->PickWeapon(m_iWeaponType);
      SetInactive();

      GameServer::Event event;
      event.type = EVENT_ITEM_PICKUP;
      event.pos = m_vPosition;
      m_pGameServer->AddEvent(event);
   }
   // weapon is not ammo full
   else
   {
      Weapon* pWp = m_pCharTriggered->WeaponSys()->getWeaponFromInventory(m_iWeaponType);
      if ( pWp->AmmoCount() < pWp->MaxAmmoCount() )
      {
         pWp->getFullAmmo();
         SetInactive();
         GameServer::Event event;
         event.type = EVENT_ITEM_PICKUP;
         event.pos = m_vPosition;
         m_pGameServer->AddEvent(event);
      }
   }
}
