#include "Giblet.h"
#include <base/Time.h>
#include <engine/client/gfx/GFXAsset.h>
#include <engine/client/sfx/SFXAsset.h>
#include <game/server/Tile.h>
#include "Effects.h"
#include "ParticleManager.h"

Giblet::Giblet(ParticleManager& particleMgr,
               Vector2D pos,
               Vector2D sprSize,
               std::vector<float> mapping,
               float curAngle,
               float angleSpeed,
               Vector2D v0) :

   Particle(
            4000,
            TEX_PARTICLES,
            sprSize,
            sprSize,
            curAngle,
            angleSpeed,
            Time::ms(),
            Tile::m_fHEIGHT*0.8f,
            v0,
            true,
            0.5f,
            true,
            std::vector<float>(4, 1.0f),
            std::vector<float>(4, 1.0f),
            pos,
            sprSize*0.8,
            SND_GIB_IMPACT_1,
            SND_GIB_IMPACT_3),

   m_ParticleMgr(particleMgr),
   m_iTimeBetweenSplatter(50),
   m_iLastTimeSplatter(Time::ms())
{
   m_sprite.setTexMapping( mapping );
   m_sprite.setVisible();

   m_endColor[3] = 0.0f;
   m_bChangingColor = true;

}

void Giblet::updateSpecial()
{
   // handle little gib splatter
   int curTime = Time::ms();
   if ( (curTime-m_iLastTimeSplatter >= m_iTimeBetweenSplatter) && (LifeTime() < 1000) )
   {
      float velFactor = 1.25f;
      Vector2D offset( m_vVelocity.GetReverse() );

      offset *= velFactor;

      Vector2D splatterPos (m_vPosition);
      splatterPos += offset;

      Particle* pPart = Effects::GibLittleSplatter(splatterPos);
      m_ParticleMgr.addParticle(pPart);

      m_iLastTimeSplatter = curTime;
   }

   // play sound only one time
   if( m_iCollisionCount >= 1 )
   {
      m_iFirstSoundId = SND_SILENCE;
      m_iLastSoundId  = SND_SILENCE;
   }

}

