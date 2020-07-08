#include "HealthGiver.h"
#include "TriggerSystem.h"
#include <game/server/GameServer.h>

#include "Character.h"
#include "Tile.h"
#include "EntityEnumerations.h"

#include "Map.h"


HealthGiver::HealthGiver(GameServer *pGameServer, TriggerSystem* pTrigSys, Vector2D pos) :
   PowerupGiver(pGameServer, pTrigSys, pos, PW_HEALTH)
{
	m_Type = TG_ITEM_POWERUP;
}


void HealthGiver::DoAction()
{
   // character don't take health if he has maximum amount health
   if(m_pCharTriggered->GetLife() < m_pCharTriggered->GetMaxLife())
   {
      m_pCharTriggered->addLife(20);
      SetInactive();

      GameServer::Event event;
      event.type = EVENT_ITEM_PICKUP;
      event.pos = m_vPosition;
      m_pGameServer->AddEvent(event);
   }
}
