#include "Waypoint.h"
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <game/server/Tile.h>

Waypoint::Waypoint(int idx, Tile* pSpotTile) : GraphNode(idx),
  m_vPosition(pSpotTile->Position().x + Tile::m_fWIDTH/4,
              pSpotTile->Position().y + Tile::m_fHEIGHT/4),
  m_bNextToReach(false)
{}


const Vector2D& Waypoint::Position() const { return m_vPosition; }

bool Waypoint::isNextToReach()
{
   return m_bNextToReach;
}

void Waypoint::SetNextToReach(bool b)
{
   m_bNextToReach = b;
}
