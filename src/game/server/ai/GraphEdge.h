#ifndef GRAPHEDGE_H_INCLUDED
#define GRAPHEDGE_H_INCLUDED

#include <GL/glew.h>
#include <ostream>
#include <fstream>

class Vector2D;

///
/// \class GraphEdge GraphEdge.h
/// \brief This class implements sparse graph edge
///
///
class GraphEdge
{
   public:

      /// \brief Constructs graph edge from node \a from to node \a to with cost \a cost
      GraphEdge(int from, int to, float cost);

      /// \brief Constructs graph edge from node \a from to node \a to with cost equals 1.0
      GraphEdge(int from, int  to);

      /// \brief Constructs graph edge with invalid node indexes and cost equals 1.0
      GraphEdge();

      virtual ~GraphEdge();

      int   From() const;
      void  SetFrom(int NewIndex);

      int   To()const;
      void  SetTo(int NewIndex);

      float Cost()const;
      void  SetCost(float NewCost);

      virtual void render(bool renderLarge, Vector2D& fromNodePos, Vector2D& toNodePos) = 0;

      bool operator==(const GraphEdge& rhs);
      bool operator!=(const GraphEdge& rhs);

      //for reading and writing to streams.
      friend std::ostream& operator<<(std::ostream& os, const GraphEdge& e);

   protected:

      float    m_fCost;  ///< the cost of traversing the edge
      int      m_iFrom;  ///< source node
      int      m_iTo;    ///< destination node

};




#endif // GRAPHEDGE_H_INCLUDED
