#ifndef GOALEVALUATORGETHEALTH_H
#define GOALEVALUATORGETHEALTH_H

#include "GoalEvaluator.h"
#include <game/server/Bot.h>

///
/// \class GoalEvaluatorGetHealth GoalEvaluatorGetHealth.h
/// \brief calculate how desirable the goal of fetching a health item
///  is
///
class GoalEvaluatorGetHealth : public GoalEvaluator
{
   public:

      GoalEvaluatorGetHealth(float bias);

      /// \brief desirability calcul based on health and distance to reach item
      float CalculateDesirability(Bot* pBot);
      void  SetGoal(Bot* pEnt);
};


#endif // GOALEVALUATORGETHEALTH_H
