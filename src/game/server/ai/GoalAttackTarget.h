#ifndef GOALATTACKTARGET_H
#define GOALATTACKTARGET_H

#include "GoalComposite.h"

class Bot;

///
/// \class GoalAttackTarget GoalAttackTarget.h
/// \brief A goal that does an target attack
///
/// Target attack can be to atttack with dodge or hunt target.
///
class GoalAttackTarget : public GoalComposite<Bot>
{
   public:

      GoalAttackTarget(Bot* pOwner);

      void Activate();
      int  Process();
      void Terminate();
};

#endif // GOALATTACKTARGET_H
