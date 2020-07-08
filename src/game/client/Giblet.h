#ifndef GIBLET_H
#define GIBLET_H

#include "Particle.h"

class ParticleManager;

class Giblet : public Particle
{
   public:
      //enum GibletType {HEAD, TORSO, ARM, FORWARDARM, HIGHLEG, LOWLEG};

      Giblet(ParticleManager& particleMgr,
               Vector2D pos,
               Vector2D sprSize,
               std::vector<float> mapping,
               float curAngle,
               float angleSpeed,
               Vector2D v0
               );

      Giblet(const Giblet& g);

   protected:

      void updateSpecial();

      int m_iLastTimeSplatter; //TODO repeat a task each x ms should be done by an advanced timer
      int m_iTimeBetweenSplatter;

      ParticleManager& m_ParticleMgr;
};

#endif // GIBLET_H
