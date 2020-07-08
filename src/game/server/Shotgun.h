#ifndef SHOTGUN_H
#define SHOTGUN_H


#include "Weapon.h"

class WeaponSystem;

class Shotgun : public Weapon
{
   public:
      Shotgun(WeaponSystem* pws);
      void Fire() const;
};

#endif // SHOTGUN_H
