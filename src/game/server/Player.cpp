#include "Player.h"
#include <iostream>
#include <fstream>
#include <game/server/GameServer.h>
#include "WeaponSystem.h"
#include "PlayerWeaponSystem.h"
#include "EntityEnumerations.h"
#include <base/debug.h>
#include <engine/shared/Common.h>

Player::Player(GameServer* pGameServer, int clientId) : Character(pGameServer, clientId)
{
   assert( pGameServer != 0 );
   assert( clientId >= 0 && clientId < MAX_CLIENTS );

   m_pWeaponSys = new PlayerWeaponSystem(m_pGameServer, this);
}

bool Player::isDoingLeftMove() const
{
	return m_aInputs[INPUT_LEFT];
}

bool Player::isDoingRightMove() const
{
	return m_aInputs[INPUT_RIGHT];
}

bool Player::isDoingJump()
{
	return m_aInputs[INPUT_JUMP];
}

bool Player::IsJetpacking() const
{
	return m_aInputs[INPUT_JETPACK];
}

void Player::updateSpecial()
{
   UpdatePos();
   m_pWeaponSys->Update();
}

