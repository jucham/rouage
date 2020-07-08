#ifndef WEAPON_H
#define WEAPON_H

#include <vector>
#include "EntityEnumerations.h"

class WeaponSystem;

struct Dimension
{
      Dimension() :
            width(0.0f),
            height(0.0f)
      {
      }

      Dimension(float w, float h) :
            width(w),
            height(h)
      {
      }

      float width;
      float height;
};

class Weapon
{
   public:
      Weapon(WeaponSystem* pws, int type);
      virtual ~Weapon();
      void Reset();
      void DoFire();
      virtual void Fire() const = 0;
      int getType() const;
      bool Ready();
      int AmmoCount() const;
      bool NoAmmo() const;
      int MaxAmmoCount() const;
      void getFullAmmo();
      void update();
      float HandOffset() const;
      float Width() const
      {
         return m_fWidth;
      }
      float Height() const
      {
         return m_fHeight;
      }

      void ResetTimeDelay();

      static Dimension m_fWeaponDims[NUM_WEAPONS];

   protected:
      WeaponSystem* m_pWeaponSys;
      float m_fWidth;
      float m_fHeight;
      int m_iType;
      int m_TimeNextAvailable;
      int m_DelayBetweenShots;
      int m_AmmoCount;
      int m_MaxAmmoCount;
      float m_fHandOffset; ///< projectile will fire from weapon position + projOffset

   public:
      float m_fProjCannonOffset; ///< let to adjust projectile to that well comes out of the cannon

};

#endif // WEAPON_H
