#ifndef GOALEVALUATORATTACK_H
#define GOALEVALUATORATTACK_H

#include "GoalEvaluator.h"

class Bot;

///
/// \class GoalEvaluatorAttack GoalEvaluatorAttack.h
/// \brief This class implements evaluator for attack strategy
///
class GoalEvaluatorAttack : public GoalEvaluator
{
   public:

      GoalEvaluatorAttack(float bias);

      /// \brief calcul is based on health and total weapon strength.
      float CalculateDesirability(Bot* pBot);
      void  SetGoal(Bot* pEnt);
};

#endif // GOALEVALUATORATTACK_H
