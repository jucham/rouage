#ifndef AASLOADER_H
#define AASLOADER_H

#include <list>
#include <vector>
#include <base/FileLoader.h>

struct NodeData
{
   int idx;
   int x;
   int y;
};

struct EdgeData
{
   int src;
   int dst;
   float cost;
   int action; // walk, jump, etc.
};

struct AASItemData
{
   int idx; // node index
   int x;
   int y;
   int itemType;
   int type;
};

// AAS aka Area Awareness System, maybe rename that
class AASLoader : public FileHandler
{
   public:
      AASLoader(const char* fileName);
      ~AASLoader();

      void ReadHeader();
      void readData();
      void readNodeData();
      void readEdgeData();
      void readItemData();
      void readTableData();

      const std::vector<NodeData>& getNodeData() const;
      const std::vector<EdgeData>& getEdgeData() const;
      const std::vector<AASItemData>& getItemData() const;

      const std::vector< std::vector<int> >& getPathsTable() const;
      const std::vector< std::vector<float> >& getCostsPathsTable() const;

   protected:

      std::vector<NodeData>     m_aNodeData;
      std::vector<EdgeData>     m_aEdgeData;
      std::vector<AASItemData>  m_aItemData;

      std::vector< std::vector<int> >     m_shortestPathsTable;
      std::vector< std::vector<float> >   m_shortestPathsCostsTable;

};

#endif // AASLOADER_H
