#include "GraphNode.h"


GraphNode::GraphNode() :
   m_iIndex( -1 )
{}

GraphNode::GraphNode(int idx) :
   m_iIndex(idx)
{}

GraphNode::~GraphNode() {}

int GraphNode::Index() const {return m_iIndex;}

void GraphNode::SetIndex(int NewIndex) {m_iIndex = NewIndex;}

std::ostream& operator<<(std::ostream& os, const GraphNode& n)
{
   os << "Index: " << n.m_iIndex << std::endl; return os;
}
