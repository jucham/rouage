#include "GoalThink.h"

#include <list>

#include <game/server/EntityEnumerations.h>
#include <game/server/Tile.h>
#include <game/server/Map.h>
#include <game/server/GameServer.h>
#include "GoalExplore.h"
#include "GoalAttackTarget.h"
#include "GoalGetItem.h"
#include "GoalTypes.h"
#include "GoalEvaluator.h"
#include "GoalEvaluatorGetHealth.h"
#include "GoalEvaluatorExplore.h"
#include "GoalEvaluatorGetWeapon.h"
#include "GoalEvaluatorAttack.h"
#include "NavGraphEdge.h"


GoalThink::GoalThink(Bot* pBot) :
   GoalComposite<Bot>(pBot, GOAL_THINK),
   m_Evaluators()
{
   //these biases could be loaded in from a script on a per bot basis
   //but for now we'll just give them some random values
   const float LowRangeOfBias = 0.5f;
   const float HighRangeOfBias = 1.5f;

   float ExploreBias = 1.0f;//RandInRange(LowRangeOfBias, HighRangeOfBias);
   float AttackBias = 1.0f;//RandInRange(LowRangeOfBias, HighRangeOfBias);
   float HealthBias = 1.0f;//RandInRange(LowRangeOfBias, HighRangeOfBias);
   float RocketLauncherBias = 1.0f;//RandInRange(LowRangeOfBias, HighRangeOfBias);
   float PistolBias = 1.0f;//RandInRange(LowRangeOfBias, HighRangeOfBias);

   //create the evaluator objects
   m_Evaluators.push_back( new GoalEvaluatorExplore(ExploreBias) );
   m_Evaluators.push_back( new GoalEvaluatorAttack(AttackBias) );
   m_Evaluators.push_back( new GoalEvaluatorGetHealth(HealthBias) );
   m_Evaluators.push_back( new GoalEvaluatorGetWeapon(RocketLauncherBias, WP_ROCKET_LAUNCHER) );
   m_Evaluators.push_back( new GoalEvaluatorGetWeapon(PistolBias, WP_PISTOL) );

}


GoalThink::~GoalThink()
{
  GoalEvaluators::iterator curDes = m_Evaluators.begin();
  for (curDes; curDes != m_Evaluators.end(); ++curDes)
  {
    delete *curDes;
  }
}


void GoalThink::Activate()
{
  Arbitrate();
  m_iStatus = active;
}

int GoalThink::Process()
{
   #if DBG
      dbg << "class = " << typeid(*this).name() << std::endl;
   #endif
   ActivateIfInactive();

   int SubgoalStatus = ProcessSubgoals();

   if (SubgoalStatus == completed || SubgoalStatus == failed)
   {
      m_iStatus = inactive;
   }

  return m_iStatus;
}

void GoalThink::Terminate() {}

//  this method iterates through each goal option to determine which one has
//  the highest desirability.
void GoalThink::Arbitrate()
{

   float best = 0;
   GoalEvaluator* MostDesirable = 0;

   //iterate through all the evaluators to see which produces the highest score
   GoalEvaluators::iterator curDes = m_Evaluators.begin();
   for (curDes; curDes != m_Evaluators.end(); ++curDes)
   {
      float desirability = (*curDes)->CalculateDesirability(m_pOwner);
      (*curDes)->m_fLastDesirability = desirability;

      if (desirability >= best)
      {
         best = desirability;
         MostDesirable = *curDes;
      }
   }

   assert (MostDesirable && "<GoalThink::Arbitrate>: no evaluator selected");

   MostDesirable->SetGoal(m_pOwner);
}

//  returns true if the goal type passed as a parameter is the same as this
//  goal or any of its subgoals
bool GoalThink::notPresent(unsigned int GoalType)const
{
   if (!m_SubGoals.empty())
   {
      return m_SubGoals.front()->GetType() != GoalType;
   }

   return true;
}

void GoalThink::AddGoalExplore()
{
   if (notPresent(GOAL_EXPLORE))
   {
      RemoveAllSubgoals();
      AddSubgoal( new GoalExplore(m_pOwner));
   }
}

void GoalThink::AddGoalGetItem(unsigned int itemType, int type)
{
   if (notPresent(ItemTypeToGoalType(itemType, type)))
   {
      RemoveAllSubgoals();
      AddSubgoal( new GoalGetItem(m_pOwner, itemType, type));
   }
}

void GoalThink::AddGoalAttackTarget()
{
   if (notPresent(GOAL_ATTACK_TARGET))
   {
      RemoveAllSubgoals();
      AddSubgoal( new GoalAttackTarget(m_pOwner));
   }
}

