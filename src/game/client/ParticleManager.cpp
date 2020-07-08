#include "ParticleManager.h"

#include "GameClient.h"
#include <base/utils.h>

extern int g_numGameEntity;

ParticleManager::ParticleManager(const Camera &cam) :
   m_particles(),
   m_corpses(),
   m_allParticles(),
   m_vaParticles(0),
   m_vaCorpses(0),
   m_vboParticles(m_PARTICLES_FLOAT_SIZE*sizeof(float), GL_STREAM_DRAW),
   m_vboCorpses(m_CORPSES_FLOAT_SIZE*sizeof(float), GL_STREAM_DRAW),
   m_Cam(cam)
{
   m_vaParticles = new float[m_PARTICLES_FLOAT_SIZE];
   m_vaCorpses = new float[m_CORPSES_FLOAT_SIZE];
}

ParticleManager::~ParticleManager()
{
   // delete classic particles
   std::list<Particle*>::iterator pCurPart = m_particles.begin();
   while ( pCurPart != m_particles.end() )
   {
         delete *pCurPart;
         ++pCurPart;
   }
   m_particles.clear();

   // delete corpse particles
   pCurPart = m_corpses.begin();
   while ( pCurPart != m_corpses.end() )
   {
         delete *pCurPart;
         ++pCurPart;
   }
   m_corpses.clear();

   // delete vertex arrays
   delete[] m_vaParticles;
   delete[] m_vaCorpses;
}


void ParticleManager::addParticle(Particle* pPart)
{
   pPart->setStartTime( Time::ms() );
   m_particles.push_back( pPart );
}

void ParticleManager::addParticles(std::list<Particle*> partList)
{
   int curTime = Time::ms();

   std::list<Particle*>::iterator pCurPart;
   for(pCurPart = partList.begin(); pCurPart != partList.end(); ++pCurPart)
   {
      Particle* pPart = *pCurPart;
      pPart->setStartTime(curTime);
      m_particles.push_back( pPart );
   }
}

void ParticleManager::addCorpse(Particle* pCorpse)
{
   pCorpse->setStartTime( Time::ms() );
   m_corpses.push_back( pCorpse );
}

void ParticleManager::updateParticles()
{
   // update classic particule
   std::list<Particle*>::iterator pCurPart = m_particles.begin();
   while ( pCurPart != m_particles.end() )
   {
      Particle* pPart = *pCurPart;

      if ( pPart->isDead() )
      {
         delete pPart;
         pCurPart = m_particles.erase(pCurPart);
         continue;
      }

      pPart->Update();
      ++pCurPart;
   }

//   // update corpse particule
//   std::list<Particle*>::iterator pCurCorpse = m_corpses.begin();
//   while ( pCurCorpse != m_corpses.end() )
//   {
//      Particle* pCorpse = *pCurCorpse;
//
//      if ( ! pCorpse->isDead() )
//      {
//         pCorpse->update();
//         ++pCurCorpse;
//      }
//      else
//      {
//         delete pCorpse;
//         pCurCorpse = m_corpses.erase(pCurCorpse); // efface et se positionne sur le prochain elm
//      }
//
//   }

   // build container with all particles concatened
   m_allParticles.erase( m_allParticles.begin(), m_allParticles.end() );
   m_allParticles.insert(m_allParticles.end(), m_particles.begin(), m_particles.end());
//   m_allParticles.insert(m_allParticles.end(), m_corpses.begin(), m_corpses.end());
}

void ParticleManager::RenderParticles()
{

   // --- render corpse particles ---
//   std::memset(m_vaCorpses, 0, m_CORPSES_FLOAT_SIZE * sizeof(float)); // on reset la mémoire
//
   int i=0;
   std::vector<int> texIdxs(m_corpses.size(), TEX_NULL);
//   std::list<Particle*>::iterator pCurCorpse;
//
//   for (pCurCorpse = m_corpses.begin();
//         pCurCorpse != m_corpses.end(); ++pCurCorpse)
//   {
//      Corpse* pCorpse = static_cast<Corpse*> (*pCurCorpse); // cast OK
//
//      // fill the vertex array with fresh data
//      pCorpse->prepareRender(m_vaCorpses, m_QUAD_SIZE*i);
//
//      texIdxs[i] = (pCorpse->isPlayer()) ? TEX_PLAYER : TEX_BOT;
//      i++;
//   }
//

//   // render corpses
//   for (int i=0; i<m_corpses.size(); i++)
//   {
//      float* ptrCurCorpse = m_vaCorpses + m_QUAD_SIZE*i;
//      std::vector<int> curTexIdx(1, texIdxs[i]);
//
//      Sprite::renderSpriteSet( m_pGame->getGfxAsset(),
//                              ptrCurCorpse,
//                              m_vboCorpses,
//                              curTexIdx,
//                              4);
//   }


   // --- render classic particles ---
   std::memset(m_vaParticles, 0, m_PARTICLES_FLOAT_SIZE * sizeof(float));

   i=0;
   std::list<Particle*>::iterator pCurPart;
   texIdxs.resize(1);
   texIdxs[0] = TEX_PARTICLES;

   for (pCurPart = m_particles.begin(); pCurPart != m_particles.end(); ++pCurPart)
   {
      // render particule only if it's into camera field of view
      if (m_Cam.isIntoFOV((*pCurPart)->Position()))
      {
    	  (*pCurPart)->prepareRender(m_vaParticles, m_QUAD_SIZE*i);
          i++;
      }
   }

   int renderedParticles = i+1;
   Sprite::renderSpriteSet(m_vaParticles, m_vboParticles, texIdxs, 4*renderedParticles);
}
