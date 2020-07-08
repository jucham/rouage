#include "GoalEvaluatorGetWeapon.h"

#include <string>

#include <game/server/EntityEnumerations.h>
#include <game/server/GameServer.h>
#include <game/server/Map.h>
#include <game/server/Bot.h>
#include "GoalThink.h"
#include "GoalTypes.h"
#include "Feature.h"


GoalEvaluatorGetWeapon::GoalEvaluatorGetWeapon(float bias, int WeaponType) :
   GoalEvaluator(bias, "Eval. Get weapon"),
   m_iWeaponType(WeaponType)
{}


float GoalEvaluatorGetWeapon::CalculateDesirability(Bot* pBot)
{
  //grab the distance to the closest instance of the weapon type
  float Distance = Feature::DistanceToItem(pBot, IT_WEAPON, m_iWeaponType);

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
    const float Tweaker = 0.15f;

    float Health, WeaponStrength;

    Health = Feature::Health(pBot);

    WeaponStrength = Feature::IndividualWeaponStrength(pBot, m_iWeaponType);

    float Desirability = (Tweaker * Health * (1-WeaponStrength)) / Distance;

    //ensure the value is in the range 0 to 1
    Clamp(Desirability, 0, 1);

    Desirability *= m_fCharacterBias;

    return Desirability;
  }
}

void GoalEvaluatorGetWeapon::SetGoal(Bot* pBot)
{
  pBot->getBrain()->AddGoalGetItem(IT_WEAPON, m_iWeaponType);
}

