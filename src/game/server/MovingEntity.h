#ifndef MOVINGENTITY_H_INCLUDED
#define MOVINGENTITY_H_INCLUDED

#include <base/AABBox2D.h>
#include <base/Vector2D.h>
#include "BaseGameEntity.h"
#include "EntityEnumerations.h"

class GameServer;

class MovingEntity: public BaseGameEntity
{
   public:
      MovingEntity(GameServer* pGameServer, int startFlyingTime, Vector2D flyForce, float g);

      void updateVelocity();
      void updatePosition();

      virtual void onCollide(EntityType entType, OrientHV ori)=0;

      void setGravity(float coef);
      const Vector2D& Velocity() const;
      AABBox2D* BBox();

      const Vector2D CenterPos() const;

   protected:
      GameServer* m_pGameServer;
      Vector2D m_vVelocity;
      AABBox2D m_HitBox;
      int m_iStartFlyingTime;
      Vector2D m_vV0;
      float m_fG;
};

inline const Vector2D MovingEntity::CenterPos() const
{
   return Vector2D(m_vPosition.x + m_HitBox.Width() / 2, m_vPosition.y + m_HitBox.Height() / 2);
}

#endif // MOVINGENTITY_H_INCLUDED
