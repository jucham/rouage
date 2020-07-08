#include "GoalWalkOnEdge.h"

#include <game/server/Bot.h>
#include "NavGraphEdge.h"


GoalWalkOnEdge::GoalWalkOnEdge(Bot* pBot, NavGraphEdge* edge) :
   GoalActionOnEdge(pBot, edge, GOAL_WALK_ON_EDGE)
{}


void GoalWalkOnEdge::Activate()
{
   m_iStatus = active;
}

int GoalWalkOnEdge::Process()
{
   #if DBG
      dbg << "class = " << typeid(*this).name() << std::endl;
   #endif

   //if status is inactive, call Activate()
   ActivateIfInactive();

   m_pOwner->putInRightDirection(m_rWaypointToReach);

   int blockingState = m_pOwner->BlockingState();

   if ( blockingState == BLOCKED_CAN_UNLOCK )
   {
      if (m_pOwner->triedToUnlock() )
      {
         m_iStatus = completed;
      }
      else
      {
         m_pOwner->doingJump_On();
         m_pOwner->oneTryToUnlock();
      }
   }
   else if (blockingState == BLOCKED_CAN_NOT_UNLOCK) {
      m_iStatus = completed;
   }

   else if ( m_pOwner->isTouchingWaypoint(m_rWaypointToReach) )
   {
      unhighlightEdge();
      m_rWaypointToReach.SetNextToReach(false);
      m_iStatus = completed;
   }

  return m_iStatus;
}


void  GoalWalkOnEdge::Terminate()
{
   m_pOwner->moveOff();
}

