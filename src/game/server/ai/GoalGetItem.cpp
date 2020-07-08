#include "GoalGetItem.h"

#include <game/server/EntityEnumerations.h>
#include <game/server/Bot.h>
#include <game/server/GameServer.h>
#include <game/server/Map.h>
#include "PathPlanner.h"
#include "GoalMoveToWaypoint.h"
#include "GoalFollowPath.h"

#include <typeinfo>

int ItemTypeToGoalType(int itemType, int type)
{
   switch(itemType)
   {
      case IT_POWERUP :
      {
         switch(type)
         {
            case PW_HEALTH :
               return GOAL_GET_HEALTH;
         }
      }
      break;

      case IT_WEAPON :
      {
         switch(type)
         {
            case WP_PISTOL :
               return GOAL_GET_PISTOL;

            case WP_ROCKET_LAUNCHER :
               return GOAL_GET_ROCKET_LAUNCHER;
         }
      }
      break;

   }//end switch

  throw std::runtime_error("GoalGetItem cannot determine item type");
}

GoalGetItem::GoalGetItem(Bot* pBot, int itemType, int type) :
   GoalComposite<Bot>(pBot, ItemTypeToGoalType(itemType, type)),
   m_iItemType(itemType),
   m_iType(type),
   m_pGiverTrigger(0)
{}


void GoalGetItem::Activate()
{
   m_iStatus = active;
   RemoveAllSubgoals();
   m_pGiverTrigger = 0;

   PathPlanner& pathPlnr = m_pOwner->GetGameServer()->GetMap()->GetPathPlanner();
   int from = pathPlnr.getClosestNode(m_pOwner->Position());
   int to = pathPlnr.getClosestNodeToItem(from, m_iItemType, m_iType);

   if(to == -1)
   {
      m_iStatus = failed;
      return;
   }

   m_pGiverTrigger = pathPlnr.getTrigger(m_iItemType, m_iType, to);

   // bot is near of waypoint, no need path
   if (from == to)
   {
      AddSubgoal( new GoalMoveToWaypoint(m_pOwner, m_pOwner->GetGameServer()->GetMap()->GetNavGraph().GetNode(to)));
   }

   // need path
   else
   {
      std::list<NavGraphEdge*> path = pathPlnr.getShortestPath(from, to);
      AddSubgoal( new GoalFollowPath(m_pOwner, path));
   }
}

int GoalGetItem::Process()
{
   #if DBG
      dbg << "class = " << typeid(*this).name() << std::endl;
   #endif

   ActivateIfInactive();

   if (hasItemBeenStolen())
   {
      Terminate();
   }

   else
   {
      //process the subgoals
      m_iStatus = ProcessSubgoals();
   }

   return m_iStatus;
}

void GoalGetItem::Terminate()
{
   m_iStatus = completed;
}

//  returns true if the bot sees that the item it is heading for has been
//  picked up by an opponent
//
bool GoalGetItem::hasItemBeenStolen()const
{
  if (m_pGiverTrigger &&
      !m_pGiverTrigger->isActive())
  {
    return true;
  }

  return false;
}
