#include "GoalMoveToWaypoint.h"

#include <game/server/Bot.h>
#include "Waypoint.h"

GoalMoveToWaypoint::GoalMoveToWaypoint(Bot* pBot, Waypoint& rWpt) :
   Goal<Bot>(pBot, GOAL_MOVE_TO_WAYPOINT),
   m_rWaypointToReach( rWpt )
{
   // sets rendering
   m_rWaypointToReach.SetNextToReach(true);
}

void GoalMoveToWaypoint::Activate()
{
   m_iStatus = active;
}

int GoalMoveToWaypoint::Process()
{
   #if DBG
      dbg << "class = " << typeid(*this).name() << std::endl;
   #endif
   //if status is inactive, call Activate()
   ActivateIfInactive();

   #if DBG
      dbg << "putInRightDirection " << typeid(*this).name() << std::endl;
   #endif

   m_pOwner->putInRightDirection(m_rWaypointToReach);


#if DBG
      dbg << "get BlockingState " << typeid(*this).name() << std::endl;
   #endif
   int blockingState = m_pOwner->BlockingState();


#if DBG
      dbg << "test BlockingState" << typeid(*this).name() << std::endl;
   #endif
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
      m_rWaypointToReach.SetNextToReach(false);
      m_iStatus = completed;
   }


   #if DBG
      dbg << "end process :" << typeid(*this).name() << std::endl;
   #endif
  return m_iStatus;
}

void  GoalMoveToWaypoint::Terminate()
{
   m_pOwner->moveOff();
}
