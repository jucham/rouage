#include "Particle.h"
#include <game/client/GameClient.h>
#include <game/server/Tile.h>
#include "Renderer.h"

const float Particle::GROUND_FRICTION_FACTOR = 0.98f;

Particle::Particle(
                     int lifeSpan,
                     int sprTexId,
                     Vector2D sprStartSize,
                     Vector2D sprEndSize,
                     float curAngle,
                     float angleSpeed,
                     int startFlyingTime,
                     float g,
                     Vector2D v0,
                     bool mustBounce,
                     float bounceCoef,
                     bool mustBeCollided,
                     std::vector<float> startColor,
                     std::vector<float> endColor,
                     const Vector2D& pos,
                     Vector2D boxSize,
                     int firstSoundId,
                     int lastSoundId) :

                     //TODO remove GameServer dependency in Movingentity
      MovingEntity(0,
                   startFlyingTime,
                   Vector2D(),
                   g),

      m_iLifeSpan(lifeSpan),
      m_iStartTime(0),
      m_vStartSize(Renderer::MeterToPixel(sprStartSize)),
      m_vEndSize(Renderer::MeterToPixel(sprEndSize)),
      m_fCurAngle(curAngle),
      m_fAngleSpeed(angleSpeed),
      m_bMustBounce(mustBounce),
      m_fBounceCoef( bounceCoef),
      m_bMustBeCollided(mustBeCollided),
      m_bScaling( !isEqual(m_vStartSize.x, m_vEndSize.x) || !isEqual(m_vStartSize.y, m_vEndSize.y)),
      m_startColor(startColor),
      m_endColor(endColor),
      m_currentColor(startColor),
      m_bChangingColor(false),
      m_bGroundFriction(true),
      m_iFirstSoundId(firstSoundId),
      m_iLastSoundId(lastSoundId),
      m_iCollisionCount(0),
      m_sprite(sprStartSize.x, sprStartSize.y)
{
   m_iStartFlyingTime = startFlyingTime;
   m_fG = g;
   m_vV0 = v0;
   m_vPosition = pos;
   m_HitBox.setWidth(boxSize.x);
   m_HitBox.setHeight(boxSize.y);

//   printf("boxSize.x : %f\n", boxSize.x);
//   printf("boxSize.y : %f\n", boxSize.y);

   assert (m_HitBox.Width() < Tile::m_fWIDTH*2 && m_HitBox.Height() < Tile::m_fHEIGHT*2);

   assert (m_startColor.size() == 4);
   assert (m_endColor.size() == 4);

   // check if we must change color
   for (int i=0; i<4; i++)
   {
      if( ! isEqual(m_startColor[i], m_endColor[i]) )
      {
         m_bChangingColor = true;
         break;
      }
   }

   m_sprite.setSize(m_vStartSize);

}

Particle::~Particle()
{
   //delete m_sprite;
}

bool Particle::mustBeCollided() const
{
   return m_bMustBeCollided;
}

float Particle::Width() const { return m_vStartSize.x; }
float Particle::Height() const { return m_vStartSize.y; }

int Particle::LifeTime() const
{
   return Time::ms() - m_iStartTime;
}

bool Particle::isDead() const
{
   return LifeTime() >= m_iLifeSpan;
}

void Particle::setStartTime(int t)
{
   m_iStartTime = t;
}

void Particle::Update()
{
   // Horizontal velocity
   if (m_bGroundFriction)
   {
      if ( ( LifeTime() > 1500 ) )
      {
         m_fAngleSpeed = 0.0f;
         m_vV0.x *= GROUND_FRICTION_FACTOR;
      }
   }

   updateVelocity();
   updatePosition();

   // update rotation
   if ( ! isEqual(m_fAngleSpeed, 0.0f) )
   {
      m_fCurAngle += m_fAngleSpeed;
   }

   updateSpecial();
}

void Particle::prepareRender(float* varray, unsigned int arrayPos)
{
   float lifeRate = static_cast<float>(LifeTime()) / m_iLifeSpan;

   // --- SCALING ---

   // TODO add non-linear evolution for those params
   if(m_bScaling)
   {
      Vector2D sprSize( m_vStartSize + (m_vEndSize - m_vStartSize) * lifeRate );
      m_sprite.setSize( sprSize );
   }

   // --- COLOR ---

   if( m_bChangingColor)
   {
      float offset[4];

      for(int i=0; i<4; i++)
      {
         offset[i] = m_endColor[i] - m_startColor[i];

         if ( ! isEqual(offset[i], 0.0f) )
         {
            m_currentColor[i] = m_startColor[i] + offset[i] * lifeRate;
         }
      }

      m_sprite.setColor(m_currentColor[0],
                          m_currentColor[1],
                          m_currentColor[2],
                          m_currentColor[3]);
   }

   // --- POSITION ---

   Vector2D posInPixel(Renderer::MeterToPixel(m_vPosition));

   if ( !isEqual( m_fCurAngle, 0.0f ) || !isEqual( m_fAngleSpeed, 0.0f ) ) {
      m_sprite.setPosWithAngle( posInPixel.x, posInPixel.y, m_fCurAngle, Vector2D(0,0) );
   } else{
      m_sprite.setPosition( posInPixel.x, posInPixel.y );
   }

   m_sprite.getFlatData(varray, arrayPos);
   m_sprite.setVisible();
}

void Particle::onCollide(EntityType entType, OrientHV ori)
{
   switch (entType)
   {
      case ENTITY_TILE:

         if (ori == HORIZ)
         {
            m_vV0.x = -m_vV0.x;
         }
         else
         {

            if (m_vVelocity.y > 0.0f)
            {
               m_vV0.y = 0.0f;
            }
            else
            {
               m_vV0.y = -m_vVelocity.y * m_fBounceCoef;
            }

             m_vV0.y = -m_vVelocity.y * m_fBounceCoef;

            m_iStartFlyingTime = Time::ms();

            // little bounces must not produce sound
            if( m_iFirstSoundId != SND_SILENCE && std::fabs(m_vVelocity.y) > 0.03f )
            {
               int sndId = RandInt(m_iFirstSoundId, m_iLastSoundId);
               SoundPlayer::Instance().Play(CHN_WORLD, sndId, 2, m_vPosition, 64); //TODO un volume pour chaque particule
            }
         }
         break;

      case ENTITY_PROJECTILE:
         // nothing for the time being
         break;

      default:
         assert (false && "Wrong entity type");
         break;
   }

   m_iCollisionCount++;
}
