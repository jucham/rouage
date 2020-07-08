//TODO: COMMENT

#include "Tile.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <string>

#include "../../base/Vector2D.h"
#include "BaseGameEntity.h"
#include "EntityEnumerations.h"
#include "Projectile.h"

class Trigger;

float Tile::m_fWIDTH;
float Tile::m_fHEIGHT;

Tile::Tile(float x, float y) :
            BaseGameEntity(BaseGameEntity::GetNextValidID()),
            m_pTriggerOnto(0)
{
   m_vPosition.x = x;
   m_vPosition.y = y;
   m_bOneSideSolid = false;

   for (int i = 0; i <= 4; ++i)
   {
      m_bSideSolidity[i] = false;
   }
}


void Tile::removeProjectile(int id)
{
   std::list<Projectile*>::iterator pCurProj = m_aProjOnto.begin();
   while (pCurProj != m_aProjOnto.end())
   {
      if ((*pCurProj)->ID() == id)
      {
         m_aProjOnto.erase(pCurProj);
         break;
      }

      ++pCurProj;
   }
}

void Tile::removeProjectile(Projectile* pProj)
{
   assert(pProj != 0);
   m_aProjOnto.remove(pProj);
}

void Tile::addProjectile(Projectile* pProj)
{
   assert(pProj != 0);
   m_aProjOnto.push_back(pProj);
}

bool Tile::isSolid()
{
   return m_bOneSideSolid;
}
void Tile::setSolidity(bool isSolid)
{
   m_bOneSideSolid = isSolid;
}
bool Tile::isSolidSide(TileSide ts)
{
   return m_bSideSolidity[ts];
}
void Tile::setSideSolidity(TileSide ts, bool isSolid)
{
   m_bSideSolidity[ts] = isSolid;
}
bool Tile::isCollided()
{
   return m_bCollided;
}
void Tile::setCollided(bool isCollided)
{
   m_bCollided = isCollided;
}

void Tile::SetTrigger(Trigger* t)
{
   assert(t != 0);
   m_pTriggerOnto = t;
}

Trigger* Tile::GetTriggerOnto()
{
   return m_pTriggerOnto;
}

std::list<Projectile*>& Tile::GetProjectilesOnto()
{
   return m_aProjOnto;
}

bool Tile::hasEntityOnto(int entityType)
{
   bool hasEntity = false;

   switch (entityType)
   {
   case ENTITY_TILE:
      assert(this != 0);
      hasEntity = true;
      break;

   case ENTITY_TRIGGER:

      if (m_pTriggerOnto != 0)
      {
         hasEntity = true;
      }
      break;

   case ENTITY_PROJECTILE:

      if (!m_aProjOnto.empty())
      {
         hasEntity = true;
      }
      break;

//      case ENTITY_CHARACTER:
//         if ( !m_aCharOnto.empty()) { return true; }
//         break;

   }

   return hasEntity;
}

void Tile::dbg_entitiesOnto(std::ofstream& rs)
{
   if (!m_aProjOnto.empty())
   {
      std::list<Projectile*>::iterator pCurProj = m_aProjOnto.begin();
      while (pCurProj != m_aProjOnto.end())
      {
         rs << "ID : " << (*pCurProj)->ID() << " | x = " << m_vPosition.x << ", y = " << m_vPosition.y << " | nbProj = " << m_aProjOnto.size()
               << std::endl;
         ++pCurProj;
      }
   }
}

int Tile::tileContainsCenterColumn(float xCenter)
{
   return static_cast<int>(std::floor(xCenter / m_fWIDTH));
}

int Tile::tileContainsCenterLine(float yCenter)
{
   return static_cast<int>(std::floor(yCenter / m_fHEIGHT));
}

void Tile::Destroy() {

}
