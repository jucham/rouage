#include "GoalActionOnEdge.h"

#include "Waypoint.h"
#include <game/server/GameServer.h>
#include <base/Time.h>

GoalActionOnEdge::GoalActionOnEdge(Bot* pBot, NavGraphEdge* edge, int goalType) : Goal<Bot>(pBot, goalType),
         m_Edge(edge),
         m_rWaypointToReach( m_pOwner->GetGameServer()->GetMap()->GetNavGraph().GetNode( edge->To()) )
{
   // sets rendering
   m_rWaypointToReach.SetNextToReach(true);
}

GoalActionOnEdge::~GoalActionOnEdge() {}


void GoalActionOnEdge::unhighlightEdge()
{
   m_Edge->setOnPath(false);

   // opposite edge must be updated if is float
   if (m_pOwner->GetGameServer()->GetMap()->GetNavGraph().isEdgePresent(m_Edge->To(), m_Edge->From()))
   {
      NavGraphEdge* pOppositeEdge = m_pOwner->GetGameServer()->GetMap()->GetNavGraph().GetEdge(m_Edge->To(), m_Edge->From());
      pOppositeEdge->setOnPath(false);
   }
}
