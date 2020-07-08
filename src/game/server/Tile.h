#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED

#include <list>
#include <base/Vector2D.h>
#include "BaseGameEntity.h"
#include "Projectile.h"

class Character;
class Projectile;
class Trigger;

class Tile: public BaseGameEntity
{
   private:

      bool m_bOneSideSolid; // indicate if at least one side is solid, useful to avoid overlapping calculs.
      bool m_bCollided;
      bool m_bSideSolidity[4]; // LEFT RIGHT TOP BOTTOM

      std::list<Projectile*> m_aProjOnto;
      Trigger* m_pTriggerOnto;

   public:
      static float m_fWIDTH;
      static float m_fHEIGHT;

      Tile(float x, float y);

      enum TileSide
      {
         left,
         right,
         top,
         bottom
      };

      void removeProjectile(int id);
      void removeProjectile(Projectile* pProj);
      void addProjectile(Projectile* pProj);
      bool isSolid();
      void setSolidity(bool isSolid);
      bool isSolidSide(TileSide ts);
      void setSideSolidity(TileSide ts, bool isSolid);
      bool isCollided();
      void setCollided(bool isCollided);
      bool hasEntityOnto(int entityType);
      void SetTrigger(Trigger* t);

      Trigger* GetTriggerOnto();
      std::list<Projectile*>& GetProjectilesOnto();

      static int tileContainsCenterColumn(float xCenter);
      static int tileContainsCenterLine(float yCenter);

      void dbg_entitiesOnto(std::ofstream& rs);

      bool getSideSolidity(TileSide ts) const {
    	  return m_bSideSolidity[ts];
      }

      void Destroy();

};

#endif // TILE_H_INCLUDED
