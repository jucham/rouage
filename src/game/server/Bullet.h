#ifndef BULLET_H_
#define BULLET_H_

#include "Projectile.h"


class Bullet : public Projectile
{
   public:
      static float m_fWIDTH;
      static float m_fHEIGHT;

      Bullet(Character* pShooter, const Vector2D& facing, float magnitude, int damage=0);
      ~Bullet();

      float Width();
      float Height();
      void render();

      void updateSpecial() {}

      void playRicochetSound();

//      void onHorizontalHit();
//      void onVerticalHit();

      void onCollide(EntityType entType, OrientHV ori);
};

inline float Bullet::Width() {return m_fWIDTH;}
inline float Bullet::Height() {return m_fHEIGHT;}

#endif // BULLET_H
