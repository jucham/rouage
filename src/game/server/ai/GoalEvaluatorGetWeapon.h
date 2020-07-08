#ifndef GOALEVALUATORGETWEAPON_H
#define GOALEVALUATORGETWEAPON_H

#include "GoalEvaluator.h"


///
/// \class GoalEvaluatorGetWeapon GoalEvaluatorGetWeapon.h
/// \brief calculate how desirable the goal of fetching a weapon
///  is
///
class GoalEvaluatorGetWeapon : public GoalEvaluator
{
   public:
      GoalEvaluatorGetWeapon(float bias, int WeaponType);

      /// \brief desirability calcul based on health, weapon strength and
      /// distance to weapon
      float CalculateDesirability(Bot* pBot);
      void  SetGoal(Bot* pEnt);

   protected:

      int   m_iWeaponType; ///< type of weapon wanted

};


#endif // GOALEVALUATORGETWEAPON_H
