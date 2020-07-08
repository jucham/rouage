#ifndef GOALTHINK_H
#define GOALTHINK_H

#include <vector>
#include <string>

#include <base/Vector2D.h>
#include "GoalComposite.h"

class Bot;
class GoalEvaluator;

///
/// \class GoalThink GoalThink.h
/// \brief This class implements brain of bot, goal that is on top of the hierarchy
///
class GoalThink : public GoalComposite<Bot>
{
   public:

      GoalThink(Bot* pBot);
      ~GoalThink();

      /// \brief this method iterates through each goal evaluator and selects the one
      /// that has the highest score as the current goal
      void Arbitrate();

      /// \brief returns true if the given goal is not at the front of the subgoal list
      bool notPresent(unsigned int GoalType) const;
      int  Process();
      void Activate();
      void Terminate();
      void AddGoalGetItem(unsigned int itemType, int type);
      void AddGoalExplore();
      void AddGoalAttackTarget();
   private:

      typedef std::vector<GoalEvaluator*>   GoalEvaluators;
      GoalEvaluators  m_Evaluators; ///< vector that contains all goal evalutors of differents strategies

};

#endif // GOALTHINK_H
