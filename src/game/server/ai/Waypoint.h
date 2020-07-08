#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "GraphNode.h"
#include <base/Vector2D.h>

class Tile;

///
/// \class Waypoint Waypoint.h
/// \brief This class implements a customized graph node
///
class Waypoint : public GraphNode
{
   public:

      /// \brief constructs waypoint with an index \a idx and to tile position
      /// where it must be spotted
      Waypoint(int idx, Tile* spotTile);

      const Vector2D& Position() const;

      /// \brief indicates if that waypoint is the next on bot path
      bool isNextToReach();
      void SetNextToReach(bool b);

   protected:

      Vector2D m_vPosition;
      bool     m_bNextToReach;

};

#endif // WAYPOINT_H
