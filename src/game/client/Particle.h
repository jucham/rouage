#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <game/server/MovingEntity.h>
#include <base/Vector2D.h>
#include <engine/client/gfx/Sprite.h>
#include <engine/client/sfx/SFXAsset.h>

class Particle: public MovingEntity
{

   public:

      Particle(
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
            int firstSoundId = SND_SILENCE,
            int lastSoundId = SND_SILENCE);

            ~Particle();

            bool mustBeCollided() const;

            float Width() const;
            float Height() const;

            int LifeTime() const;
            bool isDead() const;

            void setStartTime(int t);

            virtual void Update();
            virtual void prepareRender(float* varray, unsigned int arrayPos);

            void onCollide(EntityType entType, OrientHV ori);

            Sprite& getSprite() {return m_sprite;}

         protected:

            virtual void updateSpecial()
            {
            }

            static const float GROUND_FRICTION_FACTOR;

            int m_iLifeSpan;
            int m_iStartTime;
            Vector2D m_vStartSize;
            Vector2D m_vEndSize;
            float m_fCurAngle;
            float m_fAngleSpeed;
            bool m_bMustBounce;
            float m_fBounceCoef;
            bool m_bMustBeCollided;
            bool m_bScaling;

            std::vector<float> m_startColor;
            std::vector<float> m_endColor;
            std::vector<float> m_currentColor;

            bool m_bChangingColor;
            bool m_bGroundFriction;
            int m_iFirstSoundId; // idea is choose which sound or sound set that will be played
            int m_iLastSoundId;
            int m_iCollisionCount; // indicates how many times particle collided something

            Sprite m_sprite;
      };

#endif // PARTICLE_H
