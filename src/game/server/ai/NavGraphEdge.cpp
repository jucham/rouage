#include "NavGraphEdge.h"

#include <game/server/Tile.h>

NavGraphEdge::NavGraphEdge(int from, int to, float cost, int flags = 0) :
   GraphEdge(from, to, cost),
   m_isOnPath(false),
   m_iFlags(flags)
{}

void NavGraphEdge::setOnPath(bool isOnPath) { m_isOnPath = isOnPath; }

int  NavGraphEdge::Flags() const { return m_iFlags; }

void NavGraphEdge::SetFlags(int flags) { m_iFlags = flags; }

void NavGraphEdge::render(bool renderLarge, Vector2D& fromNodePos, Vector2D& toNodePos)
{

   glLoadIdentity();

   // if two edges connect two nodes then line rendered is drawn more large
   if ( renderLarge )
   {
      // draw large
      glLineWidth(3.0f);
   }
   else
   {
      glLineWidth(1.0f);
   }

   glBegin(GL_LINES);

   if(m_isOnPath) { glColor3ub(255,149,0); }
   else           { glColor3ub(0,218,59); }

   float w = Tile::m_fWIDTH/4;
   float h = Tile::m_fHEIGHT/4;

   glVertex2d( fromNodePos.x + w, fromNodePos.y + h);
   glVertex2d( toNodePos.x + w, toNodePos.y + h);

   glEnd();
}
