#include "GraphEdge.h"

#include <base/Vector2D.h>
#include "NodeTypeEnumerations.h"

GraphEdge::GraphEdge(int from, int to, float cost) :
   m_fCost(cost),
   m_iFrom(from),
   m_iTo(to)
{}

GraphEdge::GraphEdge(int from, int  to) :
   m_fCost(1.0f),
   m_iFrom(from),
   m_iTo(to)
{}

GraphEdge::GraphEdge() :
   m_fCost(1.0f),
   m_iFrom(invalid_node_index),
   m_iTo(invalid_node_index)
{}

GraphEdge::~GraphEdge() {}

int   GraphEdge::From() const {return m_iFrom;}


void  GraphEdge::SetFrom(int NewIndex){m_iFrom = NewIndex;}


int   GraphEdge::To()const{return m_iTo;}


void  GraphEdge::SetTo(int NewIndex){m_iTo = NewIndex;}


float GraphEdge::Cost()const{return m_fCost;}


void  GraphEdge::SetCost(float NewCost){m_fCost = NewCost;}



bool GraphEdge::operator==(const GraphEdge& rhs)
{
   return   rhs.m_iFrom == this->m_iFrom &&
            rhs.m_iTo   == this->m_iTo   &&
            rhs.m_fCost == this->m_fCost;
}


bool GraphEdge::operator!=(const GraphEdge& rhs)
{
   return !(*this == rhs);
}


std::ostream& operator<<(std::ostream& os, const GraphEdge& e)
{
   os << "m_iFrom: " << e.m_iFrom << " m_iTo: " << e.m_iTo
      << " m_fCost: " << e.m_fCost << std::endl;
   return os;
}
