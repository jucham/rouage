#include "GoalEvaluatorAttack.h"

#include "Feature.h"
#include "GoalThink.h"
#include "GoalTypes.h"
#include <game/server/Bot.h>
#include "TargetingSystem.h"


GoalEvaluatorAttack::GoalEvaluatorAttack(float bias) : GoalEvaluator(bias, "Eval. Attack")
{}

float GoalEvaluatorAttack::CalculateDesirability(Bot* pBot)
{
  double Desirability = 0.0f;

  //only do the calculation if there is a target present
  if (pBot->TargetSys()->isTargetPresent())
  {
     const double Tweaker = 1.0f;

     Desirability = Tweaker *
                    Feature::Health(pBot) *
                    Feature::TotalWeaponStrength(pBot);

     //bias the value according to the personality of the bot
     Desirability *= m_fCharacterBias;
  }

  return Desirability;
}

void GoalEvaluatorAttack::SetGoal(Bot* pBot)
{
  pBot->getBrain()->AddGoalAttackTarget();
}
