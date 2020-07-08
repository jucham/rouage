#ifndef GOALTRAVERSEEDGE_H
#define GOALTRAVERSEEDGE_H

#include "GoalActionOnEdge.h"

class Bot;
class NavGraphEdge;

///
/// \class GoalWalkOnEdge GoalWalkOnEdge.h
/// \brief An atomic goal that lets bot to walk along an edge
///
/// Bot don't walk strictly along an edge because game is 2d side type
/// then certains cases it's not possible to follow an edge in a
/// perfect way (by example walk and go down a big stair)
///
class GoalWalkOnEdge : public GoalActionOnEdge
{

   public:

      GoalWalkOnEdge(Bot* pBot, NavGraphEdge* edge);

      void Activate();
      int  Process();
      void Terminate();

};

#endif // GOALTRAVERSEEDGE_H
