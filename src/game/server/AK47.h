#ifndef AK47_H
#define AK47_H

#include "Weapon.h"

class WeaponSystem;

class AK47 : public Weapon
{
   public:
      AK47(WeaponSystem* pws);

      void Fire() const;

};

#endif // AK47_H
