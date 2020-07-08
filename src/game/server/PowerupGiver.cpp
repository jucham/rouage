#include "PowerupGiver.h"

#include "TriggerSystem.h"
#include <game/server/GameServer.h>

#include "Character.h"
#include "Tile.h"
#include "EntityEnumerations.h"

PowerupGiver::PowerupGiver(GameServer *pGameServer, TriggerSystem* pTrigSys, Vector2D pos, int powerupType) :
   TriggerRespawning(pTrigSys, pGameServer),
   m_iPowerupType(powerupType)
{
   assert ( pTrigSys != 0 );
   m_vPosition = pos;
   m_HitBox.setWidth( 0.0f );
   m_HitBox.setHeight( 0.0f );
   m_HitBox.setPosition(pos);
}



