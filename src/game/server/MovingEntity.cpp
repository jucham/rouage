#include "MovingEntity.h"
#include <game/server/GameServer.h>
#include "Tile.h"
#include <base/Time.h>

MovingEntity::MovingEntity(GameServer* pGame,
                 int startFlyingTime,
                 Vector2D flyForce,
                 float g) :
      BaseGameEntity(BaseGameEntity::GetNextValidID()),
      m_pGameServer(pGame),
      m_vVelocity(),
      m_HitBox(),
      m_iStartFlyingTime(startFlyingTime),
      m_vV0(flyForce),
      m_fG(g)
{}

void MovingEntity::updateVelocity()
{
   int   curTime     =  Time::ms();
   float flyingTime  = (curTime - m_iStartFlyingTime) / 1000.0f;

   m_vVelocity.x     = m_vV0.x;
   m_vVelocity.y     = m_vV0.y - m_fG * flyingTime;
}

void MovingEntity::updatePosition()
{
   m_vOldPos2.x = m_vOldPosition.x;
   m_vOldPos2.y = m_vOldPosition.y;

   m_vOldPosition.x = m_vPosition.x;
   m_vOldPosition.y = m_vPosition.y;

   m_vPosition += m_vVelocity;
   m_HitBox.setPosition(m_vPosition);
}

void MovingEntity::setGravity(float coef)
{
   Clamp(coef, 0.0f, 1.0f);
   m_fG = Tile::m_fHEIGHT * coef;
}

const Vector2D&   MovingEntity::Velocity() const {return m_vVelocity;}
AABBox2D*         MovingEntity::BBox()    {return &m_HitBox;}

