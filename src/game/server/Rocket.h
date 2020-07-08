#ifndef ROCKET_H_
#define ROCKET_H_

#include "Projectile.h"

class Rocket : public Projectile
{
   public:
      static float m_fWIDTH;
      static float m_fHEIGHT;

      Rocket(Character* pShooter, const Vector2D& facing);
      ~Rocket();
      float Width();
      float Height();
      void updateSpecial();
      void onCollide(EntityType entType, OrientHV ori);
};

inline float Rocket::Width() {return m_fWIDTH;}
inline float Rocket::Height() {return m_fHEIGHT;}

#endif // ROCKET_H
