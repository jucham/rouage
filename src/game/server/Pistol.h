#ifndef WP_PISTOL_H
#define WP_PISTOL_H

#include "Weapon.h"

class WeaponSystem;

class Pistol : public Weapon
{
   public:
      Pistol(WeaponSystem* pws);

      void Fire() const;

};

#endif // WP_PISTOL_H
