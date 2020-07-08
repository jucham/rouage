#include "PlayerWeaponSystem.h"
#include <cmath>
#include <game/server/GameServer.h>
#include "Weapon.h"
#include <base/Vector2D.h>
#include "EntityEnumerations.h"
#include "Player.h"
#include <base/Time.h>
#include <engine/shared/AbsDim.h>

PlayerWeaponSystem::PlayerWeaponSystem(GameServer* pGame, Player* pOwner) : WeaponSystem(pGame),
   m_pOwner(pOwner)
{
   assert ( pGame != 0 );
   assert ( pOwner != 0 );
}

void PlayerWeaponSystem::Update()
{
	ChangeWeapon();

	// update position
	m_vPosition.x = m_pOwner->Position().x + AbsDim::CHAR_W * 0.5f;
	m_vPosition.y = m_pOwner->Position().y + AbsDim::CHAR_H * 0.75f;

	UpdateDirection();

	if (IsShooting() && m_pCurrentWeapon->Ready())
	{
		if (m_pCurrentWeapon->NoAmmo())
		{
			GameServer::Event event;
			event.type = EVENT_NO_AMMO;
			event.pos = WeaponPos();
			m_pGame->AddEvent(event);
		}
		else
		{
			m_pCurrentWeapon->DoFire();
		}

		m_pCurrentWeapon->ResetTimeDelay();
	}
}

bool PlayerWeaponSystem::ChangeWeapon()
{

   int oldWeaponType = m_pCurrentWeapon->getType();
   int newWeaponType = -1;

   bool isWheelUp = m_pOwner->GetInput(INPUT_PREV_WEAPON);
   bool isWheelDown = m_pOwner->GetInput(INPUT_NEXT_WEAPON);

   if(isWheelUp)
   {
      selectPrevWeapon();
      newWeaponType = m_pCurrentWeapon->getType();
      return (newWeaponType != oldWeaponType);
   }

   if (isWheelDown)
   {
      selectNextWeapon();
      newWeaponType = m_pCurrentWeapon->getType();
      return (newWeaponType != oldWeaponType);
   }

   return false;
}

void PlayerWeaponSystem::UpdateDirection()
{
	m_Direction.x = cosf(m_fWeaponAngle);
	m_Direction.y = sinf(m_fWeaponAngle);
}


bool PlayerWeaponSystem::IsShooting()
{
	return m_pOwner->GetInput(INPUT_ATTACK);
}

Character* PlayerWeaponSystem::Shooter() const
{
   return m_pOwner;
}
