#include "GoalExplore.h"

#include <sstream>

#include "GoalFollowPath.h"
#include <game/server/GameServer.h>
#include <game/server/Map.h>
#include <game/server/ai/PathPlanner.h>
#include <base/Logger.h>


GoalExplore::GoalExplore(Bot* pOwner) : GoalComposite<Bot>(pOwner, GOAL_EXPLORE)
{}

void GoalExplore::Activate()
{
   m_iStatus = active;
   RemoveAllSubgoals();

   PathPlanner& pathPlnr = m_pOwner->GetGameServer()->GetMap()->GetPathPlanner();
   int from = pathPlnr.getClosestNode(m_pOwner->Position());
   int to = pathPlnr.getRandomNode(from);

   AddSubgoal(new GoalFollowPath(m_pOwner, pathPlnr.getShortestPath(from, to)));
}

int  GoalExplore::Process()
{
   //if status is inactive, call Activate()
   ActivateIfInactive();

   m_iStatus = ProcessSubgoals();

   //if there are no subgoals present check to see if the path still has edges.
   //remaining. If it does then call activate to grab the next edge.
   if (m_iStatus == completed)
   {
      Activate();
   }

   return m_iStatus;
}

void GoalExplore::Terminate() {}
