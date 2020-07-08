#include "GoalFollowPath.h"

#include <string>
#include <sstream>

#include "GoalTypes.h"
#include "NodeTypeEnumerations.h"
#include "GoalWalkOnEdge.h"
#include "GoalJumpOnEdge.h"
#include <game/server/Bot.h>
#include <game/server/WeaponSystem.h>
#include <base/Logger.h>
#include "NavGraphEdge.h"


GoalFollowPath::GoalFollowPath(Bot* pBot, std::list<NavGraphEdge*> path) :
   GoalComposite<Bot>(pBot, GOAL_FOLLOW_PATH),
   m_path(path),
   m_pCurEdge(0)
{}

GoalFollowPath::~GoalFollowPath()
{}


void GoalFollowPath::Activate()
{
   m_iStatus = active;
   m_pCurEdge = m_path.front();

   m_pOwner->setCurrentPathEdge( m_pCurEdge );

   //remove the edge from the path
   m_path.pop_front();

   switch(m_pCurEdge->Flags())
   {
      case NavGraphEdge::normal :
      {
         AddSubgoal(new GoalWalkOnEdge(m_pOwner, m_pCurEdge));
      }
      break;

      case NavGraphEdge::jump :
      {
         AddSubgoal(new GoalJumpOnEdge(m_pOwner, m_pCurEdge));
      }
      break;

      default:
         throw std::runtime_error("<Goal_FollowPath::Activate>: Unrecognized edge type");
   }
}


int GoalFollowPath::Process()
{

   // bot can be blocked in 2 ways :
   // 1. blocked and can't be unlock
   // 2. blocked and can be unlock and he tried one time to unlock without success
   if( m_pOwner->BlockingState() == BLOCKED_CAN_NOT_UNLOCK
       || (m_pOwner->BlockingState() == BLOCKED_CAN_UNLOCK && m_pOwner->triedToUnlock()) )
   {
      m_path.clear();
      m_iStatus = completed;
      m_pOwner->setNoBlocked();
      m_pOwner->resetTryToUnlock();

      return completed;
   }

   //if status is inactive, call Activate()
   ActivateIfInactive();

   m_iStatus = ProcessSubgoals();

   //if there are no subgoals present check to see if the path still has edges.
   //remaining. If it does then call activate to grab the next edge.
   if (m_iStatus == completed && !m_path.empty())
   {
       Activate();
   }

   return m_iStatus;
}


void GoalFollowPath::Terminate()
{

}


void GoalFollowPath::render(Vector2D& pos)const
{
   int i=1;
   std::list<NavGraphEdge*>::const_iterator pCurEdge;

   //RenderTools::renderText(pos.x, pos.y, 0, 0, "{");

//   for(pCurEdge = m_path.begin(); pCurEdge!=m_path.end(); ++pCurEdge)
//   {
//      std::ostringstream oss;
//      oss << "(" << (*pCurEdge)->From() << "," << (*pCurEdge)->To() << ")";
//      RenderTools::renderText(pos.x + i*30, pos.y, 0, 0, oss.str());
//      ++i;
//   }

//   RenderTools::renderText(pos.x + ++i*30, pos.y, 0, 0, "}");
}
