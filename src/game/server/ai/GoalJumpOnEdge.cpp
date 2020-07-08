#include "GoalJumpOnEdge.h"

#include <game/server/Bot.h>
#include "NavGraphEdge.h"

GoalJumpOnEdge::GoalJumpOnEdge(Bot* pBot, NavGraphEdge* edge) :
   GoalActionOnEdge(pBot, edge, GOAL_JUMP_ON_EDGE)
{}

void GoalJumpOnEdge::Activate()
{
   m_iStatus = active;
   m_pOwner->putInRightDirection(m_rWaypointToReach);
   m_pOwner->doingJump_On();
}

int GoalJumpOnEdge::Process()
{
   #if DBG
      dbg << "class = " << typeid(*this).name() << std::endl;
   #endif

   ActivateIfInactive();

   if ( m_pOwner->isGround() )
   {
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

      else if ( m_pOwner->isTouchingWaypoint(m_rWaypointToReach) ) {
         unhighlightEdge();
         m_rWaypointToReach.SetNextToReach(false);
         m_iStatus = completed;
      }
   }

   return m_iStatus;
}

void  GoalJumpOnEdge::Terminate()
{
   m_pOwner->moveOff();
}

