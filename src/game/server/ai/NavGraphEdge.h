#ifndef NAVGRAPHEDGE_H_INCLUDED
#define NAVGRAPHEDGE_H_INCLUDED

#include "GraphEdge.h"

///
/// \class NavGraphEdge NavGraphEdge.h
/// \brief This class implements a customized graph edge
///
/// That sort of graph edge have information about behavior (walk, jump, etc.)
/// that bot must adopt on its
///
class NavGraphEdge : public GraphEdge
{
   public:

      enum
      {
         normal            = 0,
         swim              = 1 << 0,
         crawl             = 1 << 1,
         creep             = 1 << 2,
         jump              = 1 << 3,
         fly               = 1 << 4,
         grapple           = 1 << 5,
         goes_through_door = 1 << 6
      }; ///< enumerations of edge flags

      NavGraphEdge(int from, int to, float cost, int flags);

      void setOnPath(bool isOnPath);

      int  Flags() const;
      void SetFlags(int flags);

      void render(bool renderLarge, Vector2D& fromNodePos, Vector2D& toNodePos);

      bool m_isOnPath; ///< let to say if edge is on a bot current path

   protected:

      int m_iFlags; ///< value that represents behavior that bot must adopt on edge

};

#endif // NAVGRAPHEDGE_H_INCLUDED
