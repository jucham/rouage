#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "MovingEntity.h"
#include <base/Vector2D.h>
#include <base/Types.h>
#include <engine/shared/NetDataArray.h>
#include <engine/shared/Common.h>



class Character;
class Tile;
class NetProjectile;

///////////////////////////////////////////////////////////////////////////////
///
/// \class Projectile Projectile.h
/// \brief This class implements base for a projectile
///
/// A projectile is a moving entity which is fired by a weapon. It has got
/// owner who is the shooter. the tile where is projectile is registered and
/// updated when projectile is no longer onto a tile.
///
///////////////////////////////////////////////////////////////////////////////
class Projectile: public MovingEntity
{
   public:

      Projectile(Character* pShooter, const Vector2D& facing);
      virtual ~Projectile();
      void update();
      virtual void updateSpecial()=0;

      /// \fn void updateTileSpot()
      /// \brief Updates tile spots
      ///
      /// When a projectile pass into an other tile (its position is into tile)
      /// it must be updated to kwow on which tile it is
      ///
      void updateTileSpot();

      /// \fn virtual float Width()=0
      /// \brief Returns width of projectile
      ///
      /// \return a float value that represents width of projectile
      ///
      virtual float Width() {assert(false); return 0;}

      /// \fn virtual float Height()=0
      /// \brief Returns height of projectile
      ///
      /// \return a float value that represents height of projectile
      ///
      virtual float Height() {assert(false); return 0;}

      /// \fn void setDead()
      /// \brief Sets projectile as dead
      ///
      void setDead()
      {
         m_bDead = true;
      }

      /// \fn bool isDead() const
      /// \brief Indicates if projectile is dead
      ///
      /// \return true if projectile is dead
      ///
      bool isDead() const
      {
         return m_bDead;
      }

      Character* GetOwner() const
      {
         return m_pOwner;
      }

      /// \fn int Damage() const
      /// \brief Gets damage inflicted by the weapon
      ///
      int Damage() const
      {
         return m_iDamage;
      }

      /// \fn Tile* CurrentTileSpot()
      /// \brief Returns tile where is projectile
      ///
      /// \return pointer to tile where is projectile
      ///
      Tile* CurrentTileSpot();

      /// \fn void removeTileSpot()
      /// \brief Removes this projectile in projectile list of tile which indicates which
      /// projectiles are present on this tile
      ///
      void removeTileSpot();

      /// \fn Tile* dbg_curTileSpot()
      /// \brief Returns tile where is projectile
      ///
      /// \return pointer to tile where is projectile
      ///
      Tile* dbg_curTileSpot()
      {
         return m_pCurrentTileSpot;
      }

      /// \fn bool wasShotFrom(int ms)
      /// \brief indicates if projectile it was shot there was x ms or over
      ///
      /// \return true if projectile it was shot there was x ms or over
      ///
      bool wasShotFrom(int ms);

      void setWeaponType(unsigned int type);
      unsigned int WeaponType() const;

      void GetSnapshot(NetProjectile *netProjs);

      static void ResetUsedIds() {
    	  std::memset(m_UsedArrayIds, 0, MAX_PROJECTILES * sizeof(bool));
      }

      bool Exist() const {return m_Exist;}
      int ArrayId() {return m_ArrayId;}

   protected:
      int m_ArrayId; // id that let to emulate an array position
      bool m_Exist;
      static bool m_UsedArrayIds[MAX_PROJECTILES];
      unsigned int m_WeaponType;
      Character *m_pOwner;    ///< character which shoot projectile
      int m_iDamage;     ///< damages inflicted if an other character touches projectile
      bool m_bDead;       ///< indicates if projectile is dead or alive
      Vector2D m_vImpulseForce;        ///< start force that let to launch projectile
      float m_fMagnitudeFactor;     ///< multiplying base force factor (base being tile height)
      Tile *m_pOldTileSpot;         ///< tile where was projectile
      Tile *m_pCurrentTileSpot;     ///< tile where is projectile

      void setImpulseForce();
      float GetAngle() const;
      void setMagnitude(float magnitude);

      static int GetNextFreeArrayId() {
    	  for (int i=0; i<MAX_PROJECTILES; i++) {
    		  if (!m_UsedArrayIds[i]) {
    			  m_UsedArrayIds[i] = true;
    			  return i;
    		  }
    	  }
    	  assert(false && "max number of projectiles reached !");
    	  return -1;
      }


};

inline float Projectile::GetAngle() const
{
   float angle = atan(m_vVelocity.y / m_vVelocity.x);
   if (m_vVelocity.x < 0)
   {
      angle += Pi;
   }

   return angle;
}

inline void Projectile::setMagnitude(float magnitude)
{
   if (magnitude > 1.0f) m_fMagnitudeFactor = 1.0f;
   else m_fMagnitudeFactor = magnitude;
}

inline void Projectile::setWeaponType(unsigned int type)
{
   m_WeaponType = type;
}

inline unsigned int Projectile::WeaponType() const
{
   return m_WeaponType;
}

#endif // PROJECTILE_H
