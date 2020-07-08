#include "GoalEvaluatorGetHealth.h"
#include <game/server/EntityEnumerations.h>
#include "GoalThink.h"
#include "GoalTypes.h"
#include "Feature.h"


GoalEvaluatorGetHealth::GoalEvaluatorGetHealth(float bias) : GoalEvaluator(bias, "Eval. Get health") {}

float GoalEvaluatorGetHealth::CalculateDesirability(Bot* pBot)
{
   //first grab the distance to the closest instance of a health item
   float Distance = Feature::DistanceToItem(pBot, IT_POWERUP, PW_HEALTH);

   //if the distance feature is rated with a value of 1 it means that the
   //item is either not present on the map or too far away to be worth
   //considering, therefore the desirability is zero
   if (Distance == 1)
   {
      return 0;
   }
   else
   {
      //value used to tweak the desirability
      const float Tweaker = 0.2f;

      //the desirability of finding a health item is proportional to the amount
      //of health remaining and inversely proportional to the distance from the
      //nearest instance of a health item.
      float Desirability = Tweaker * (1-Feature::Health(pBot)) /
                           (Feature::DistanceToItem(pBot, IT_POWERUP, PW_HEALTH));

      //ensure the value is in the range 0 to 1
      Clamp(Desirability, 0, 1);

      //bias the value according to the personality of the bot
      Desirability *= m_fCharacterBias;

      return Desirability;
   }
}

void GoalEvaluatorGetHealth::SetGoal(Bot* pBot)
{
   pBot->getBrain()->AddGoalGetItem(IT_POWERUP, PW_HEALTH);
}
