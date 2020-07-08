#include "AASLoader.h"
#include <cassert>

AASLoader::AASLoader(const char* fileName) : FileHandler(fileName),
   m_aNodeData(),
   m_aEdgeData(),
   m_aItemData(),
   m_shortestPathsTable(),
   m_shortestPathsCostsTable()
{}
AASLoader::~AASLoader() {}


void AASLoader::ReadHeader() {}

void AASLoader::readData()
{
   readNodeData();
   readEdgeData();
   readItemData();
   readTableData();
}

void AASLoader::readNodeData()
{
   int dataSize = 0;
   m_File >> dataSize;
   m_aNodeData.resize(dataSize);

   for(int i=0; i < m_aNodeData.size(); ++i)
   {
      m_File >> m_aNodeData[i].idx;
      m_File >> m_aNodeData[i].x;
      m_File >> m_aNodeData[i].y;
   }
}

void AASLoader::readEdgeData()
{
   int dataSize = 0;
   m_File >> dataSize;
   m_aEdgeData.resize(dataSize);

   for (int i=0; i < m_aEdgeData.size(); ++i)
   {
      m_File >> m_aEdgeData[i].src;
      m_File >> m_aEdgeData[i].dst;
      m_File >> m_aEdgeData[i].cost;
      m_File >> m_aEdgeData[i].action;
   }
}

void AASLoader::readItemData()
{
   int dataSize = 0;
   m_File >> dataSize;
   m_aItemData.resize(dataSize);

   for (int i=0; i < m_aItemData.size(); ++i)
   {
      m_File >> m_aItemData[i].idx;
      m_File >> m_aItemData[i].x;
      m_File >> m_aItemData[i].y;
      m_File >> m_aItemData[i].itemType;
      m_File >> m_aItemData[i].type;
   }
}

void AASLoader::readTableData()
{
   int numNodes = m_aNodeData.size();
   assert (numNodes);


   // initialize and retrieve shortest paths table
   std::vector<int> iv(numNodes, 0);
   m_shortestPathsTable.resize(numNodes, iv);

   for (int i=0; i<numNodes; i++)
   {
      for(int j=0; j<numNodes; j++)
      {
         m_File >> m_shortestPathsTable[i][j];
      }
   }

   // initialize and retrieve shortest paths costs table
   std::vector<float> fv(numNodes, 0);
   m_shortestPathsCostsTable.resize(numNodes, fv);
   for (int i=0; i<numNodes; i++)
   {
      for(int j=0; j<numNodes; j++)
      {
         m_File >> m_shortestPathsCostsTable[i][j];
      }
   }
}

const std::vector<NodeData>& AASLoader::getNodeData() const
{
   return m_aNodeData;
}

const std::vector<EdgeData>& AASLoader::getEdgeData() const
{
   return m_aEdgeData;
}

const std::vector<AASItemData>& AASLoader::getItemData() const
{
   return m_aItemData;
}

const std::vector< std::vector<int> >& AASLoader::getPathsTable() const
{
   return m_shortestPathsTable;
}

const std::vector< std::vector<float> >& AASLoader::getCostsPathsTable() const
{
   return m_shortestPathsCostsTable;
}

