#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include <list>
#include "Particle.h"
#include <engine/client/gfx/VBO.h>

class Game;
class Camera;

class ParticleManager
{
   public:
      ParticleManager(const Camera &cam);
      ~ParticleManager();

      void addParticle(Particle* pPart);
      void addParticles(std::list<Particle*> partList);
      void addCorpse(Particle* pCorpse);
      void updateParticles();
      void RenderParticles();

      std::list<Particle*>& Particles();

   protected:
      Game*                   m_pGame;
      std::list<Particle*>    m_particles;
      std::list<Particle*>    m_corpses;
      std::list<Particle*>    m_allParticles;

      // indicates a number of bytes per quad
      // 1 quad  =  8 floats x 4 bytes  =  32 bytes
      static const unsigned int  m_QUAD_SIZE = 32;
      static const unsigned int  m_MAX_PARTICLES = 5000;
      static const unsigned int  m_PARTICLES_FLOAT_SIZE = m_QUAD_SIZE * m_MAX_PARTICLES;
      static const unsigned int  m_MAX_CORPSES = 16;
      static const unsigned int  m_CORPSES_FLOAT_SIZE = m_QUAD_SIZE * m_MAX_CORPSES;

      float*                     m_vaParticles;
      float*                     m_vaCorpses;
      VBO                        m_vboParticles;
      VBO                        m_vboCorpses;

      const Camera &m_Cam;

};

inline std::list<Particle*>& ParticleManager::Particles()
{
   return m_allParticles;
}

#endif // PARTICLEMANAGER_H
