#ifndef GOALFOLLOWPATH_H
#define GOALFOLLOWPATH_H

#include <list>
#include "GoalComposite.h"

class Bot;
class NavGraphEdge;

///
/// \class GoalFollowPath GoalFollowPath.h
/// \brief a composite goal that lets bot to follow a path
///
class GoalFollowPath : public GoalComposite<Bot>
{
   public:

      GoalFollowPath(Bot* pBot, std::list<NavGraphEdge*> path);
      ~GoalFollowPath();

      void Activate();
      int  Process();
      void Terminate();

      void render(Vector2D& pos)const;

   private:

      std::list<NavGraphEdge*>   m_path;     ///< contains path bot must follow
      NavGraphEdge*              m_pCurEdge; ///< current edge traveled

};

#endif // GOALFOLLOWPATH_H
