#ifndef GOALEVALUATOR_H_INCLUDED
#define GOALEVALUATOR_H_INCLUDED

#include <string>

class Bot;
struct Vector2D;

///
/// \class GoalEvaluator GoalEvaluator.h
/// \brief class that defines an interface for objects that are
/// able to evaluate the desirability of a specific strategy level goal
///
class GoalEvaluator
{
   public:

     GoalEvaluator(float CharacterBias, std::string name);

     virtual ~GoalEvaluator();

     /// \brief returns a score between 0 and 1 representing the desirability of the
     /// strategy the concrete subclass represents
     virtual float CalculateDesirability(Bot* pBot)=0;

     /// \brief adds the appropriate goal to the given bot's brain
     virtual void  SetGoal(Bot* pBot) = 0;

     float        m_fLastDesirability;
     std::string  m_sName; ///< name of this evaluator

  protected:

     float m_fCharacterBias; ///< give bot a personality (desirability score is multiplied by that value)

};

#endif // GOALEVALUATOR_H_INCLUDED
