#include "GoalAttackTarget.h"

#include "TargetingSystem.h"
#include "GoalDodge.h"
#include "GoalHuntTarget.h"
#include "GoalTypes.h"
#include <game/server/Bot.h>


GoalAttackTarget::GoalAttackTarget(Bot* pOwner) : GoalComposite<Bot>(pOwner, GOAL_ATTACK_TARGET)
{}


void GoalAttackTarget::Activate()
{
   m_iStatus = active;

   RemoveAllSubgoals();

   // it is possible for a bot's target to die whilst this goal is active so we
   // must test to make sure the bot always has an active target
   if (!m_pOwner->TargetSys()->isTargetPresent())
   {
      m_iStatus = completed;
      return;
   }

   // if the bot is able to shoot the target (there is LOS between bot and
   // target), then select a tactic to follow while shooting
   if (m_pOwner->TargetSys()->isTargetShootable())
   {
      AddSubgoal(new GoalDodge(m_pOwner));
   }
   //if the target is not visible, go hunt it.
   //   else
   //   {
   //      AddSubgoal(new GoalHuntTarget(m_pOwner));
   //   }
}


int GoalAttackTarget::Process()
{
  ActivateIfInactive();
  m_iStatus = ProcessSubgoals();
  ReactivateIfFailed();

  return m_iStatus;
}


void GoalAttackTarget::Terminate()
{
   m_iStatus = completed;
}
