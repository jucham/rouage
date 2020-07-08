#ifndef GOALMOVETOWAYPOINT_H
#define GOALMOVETOWAYPOINT_H

#include "Goal.h"
#include "GoalTypes.h"

class Bot;
class Waypoint;

///
/// \class GoalMoveToWaypoint GoalMoveToWaypoint.h
/// \brief An atomic goal that lets bot to move to a waypoint
///
class GoalMoveToWaypoint : public Goal<Bot>
{
   public:

      GoalMoveToWaypoint(Bot* pBot, Waypoint& rWpt);

      void Activate();
      int  Process();
      void  Terminate();

   protected:

      Waypoint& m_rWaypointToReach;
};

#endif // GOALMOVETOWAYPOINT_H
