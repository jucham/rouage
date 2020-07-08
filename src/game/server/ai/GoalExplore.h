#ifndef GOALEXPLORE_H
#define GOALEXPLORE_H

#include "GoalComposite.h"
#include "GoalTypes.h"
#include <base/Vector2D.h>
#include <game/server/Bot.h>
#include <game/server/ai/NavGraphEdge.h>

///
/// \class GoalExplore GoalExplore.h
/// \brief A composite goal that lets bot explore map
///
class GoalExplore : public GoalComposite<Bot>
{
   public:

      GoalExplore(Bot* pOwner);

      void Activate();
      int  Process();
      void Terminate();
};

#endif // GOALEXPLORE_H
