#ifndef GOALACTIONONEDGE_H
#define GOALACTIONONEDGE_H

#include "Goal.h"
#include "GoalTypes.h"
#include "NavGraphEdge.h"
#include <game/server/Bot.h>
#include <game/server/GameServer.h>
#include <game/server/Map.h>

class Bot;

///
/// \class GoalActionOnEdge GoalActionOnEdge.h
/// \brief This is general class for all atomic action on an edge (walk, jump, etc.)
///
class GoalActionOnEdge : public Goal<Bot>
{
   public:

      /// \brief Constructs action on edge \a edge with goal type \goalType
      GoalActionOnEdge(Bot* pBot, NavGraphEdge* edge, const int goalType);
      virtual ~GoalActionOnEdge();

   protected:

      /// \brief let to unhighlight edge highlighted by bot path
      void unhighlightEdge();

      NavGraphEdge*  m_Edge;
      Waypoint&      m_rWaypointToReach;

};

#endif // GOALACTIONONEDGE_H
