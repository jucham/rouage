#include "GoalEvaluatorExplore.h"

#include "Feature.h"
#include "GoalThink.h"
#include "GoalTypes.h"
#include <game/server/Bot.h>


GoalEvaluatorExplore::GoalEvaluatorExplore(float bias) : GoalEvaluator(bias, "Eval. Explore") {}


float GoalEvaluatorExplore::CalculateDesirability(Bot* pBot)
{
  float Desirability = 0.05f;

  Desirability *= m_fCharacterBias;

  return Desirability;
}


void GoalEvaluatorExplore::SetGoal(Bot* pBot)
{
  pBot->getBrain()->AddGoalExplore();
}
