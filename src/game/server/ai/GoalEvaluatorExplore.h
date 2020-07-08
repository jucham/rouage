#ifndef GOALEVALUATOREXPLORE_H
#define GOALEVALUATOREXPLORE_H

#include "GoalEvaluator.h"

class Bot;

///
/// \class GoalEvaluatorExplore GoalEvaluatorExplore.h
/// \brief This class implements evaluator for explore strategy
///
class GoalEvaluatorExplore : public GoalEvaluator
{
   public:

      GoalEvaluatorExplore(float bias);

      /// \brief value returned is very low for that others strategies be chosen
      float CalculateDesirability(Bot* pBot);
      void  SetGoal(Bot* pEnt);
};


#endif // GOALEVALUATOREXPLORE_H
