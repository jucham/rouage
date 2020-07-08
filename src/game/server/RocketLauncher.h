#ifndef ROCKETLAUNCHER_H
#define ROCKETLAUNCHER_H

#include "Weapon.h"

class WeaponSystem;

class RocketLauncher : public Weapon
{
   public:

      RocketLauncher(WeaponSystem* pws);
      void Fire() const;

};

#endif // ROCKETLAUNCHER_H
