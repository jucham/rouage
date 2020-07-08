#ifndef GOALHUNTTARGET_H
#define GOALHUNTTARGET_H

#include "GoalComposite.h"

class Bot;

///
/// \class GoalHuntTarget GoalHuntTarget.h
/// \brief a composite goal that lets bot to hunt a target
///
class GoalHuntTarget : public GoalComposite<Bot>
{
   public:
      GoalHuntTarget(Bot* pBot);

      void Activate();
      int  Process();
      void Terminate();

};

#endif // GOALHUNTTARGET_H
