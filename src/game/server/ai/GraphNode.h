#ifndef GRAPHNODE_H_INCLUDED
#define GRAPHNODE_H_INCLUDED

#include <ostream>
#include <fstream>


///
/// \class GraphNode GraphNode.h
/// \brief This class implements sparse graph node
///
class GraphNode
{
   public:

      /// \brief Constructs a graph node with an invalid node index (-1)
      GraphNode();

      /// \brief Constructs a graph node with node index \a idx
      GraphNode(int idx);

      virtual ~GraphNode();

      int  Index() const;
      void SetIndex(int NewIndex);

      //for reading and writing to streams.
      friend std::ostream& operator<<(std::ostream& os, const GraphNode& n);

   protected:

      int m_iIndex; ///< every node has an index. A valid index is >= 0

};

#endif // GRAPHNODE_H_INCLUDED
