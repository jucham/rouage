#ifndef EFFECTS_H
#define EFFECTS_H

#include <list>
#include <base/Vector2D.h>
#include "Particle.h"

class ParticleManager;

typedef std::list<Particle*> ParticleList;

class Effects
{

   public:
      static Particle*     ThrowCartridge(int weaponType, Vector2D weaponPos, Vector2D weaponDir, float weaponAngle, float weaponEjectOffset);
      static Particle*     ShotSmoke(Vector2D weaponPos, Vector2D weaponDir);
      static Particle*     ShotFlame(Vector2D weaponPos, Vector2D weaponDir, float weaponAngle, float projCannonOffset, float flameDist);
      static Particle*     RocketSmokeCloud(Vector2D pos);
      static Particle*     JetpackSmokeCloud(Vector2D pos);
      static ParticleList  RocketExplosion(Vector2D pos);
      static ParticleList  BloodSplatter(Vector2D pos);
      static Particle*     BloodCloud(Vector2D pos);
      static Particle*     GibBigSplatter(const Vector2D& pos);
      static Particle*     GibLittleSplatter(const Vector2D& pos);
      static ParticleList  ThrowGiblets(const Vector2D& pos, ParticleManager& paticleMgr);
//    static Particle*     ricochetSmoke(Game* pGame, Vector2D pos, OrientTBLR ori);
//    static ParticleList  worldDamaged(Game* pGame, Vector2D& pos, Vector2D& projDir);

};

#endif // EFFECTS_H
