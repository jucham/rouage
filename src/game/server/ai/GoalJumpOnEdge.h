#ifndef GOALJUMPONEDGE_H
#define GOALJUMPONEDGE_H

#include "GoalActionOnEdge.h"

class Bot;
class NavGraphEdge;

///
/// \class GoalJumpOnEdge GoalJumpOnEdge.h
/// \brief An atomic goal for bot jumps when he follows path
///
class GoalJumpOnEdge : public GoalActionOnEdge
{
   public:

      GoalJumpOnEdge(Bot* pBot, NavGraphEdge* edge);

      void Activate();
      int  Process();
      void Terminate();

};

#endif // GOALJUMPONEDGE_H
