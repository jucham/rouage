#include "Bullet.h"
#include <game/server/GameServer.h>
#include "EntityEnumerations.h"

float Bullet::m_fWIDTH;
float Bullet::m_fHEIGHT;

Bullet::Bullet(Character* pShooter, const Vector2D& facing, float magnitude, int damage) :
      Projectile(pShooter, facing)
{
   assert(pShooter != 0);

   m_HitBox.setWidth(m_fWIDTH);
   m_HitBox.setHeight(m_fHEIGHT);
   m_pCurrentTileSpot = CurrentTileSpot();
   m_iDamage = damage;
   setMagnitude(magnitude); //0.7;
   m_fG *= 0.3f;
   setImpulseForce();

}

Bullet::~Bullet()
{

}

void Bullet::onCollide(EntityType entType, OrientHV ori)
{
	switch (entType)
	{
		case ENTITY_TILE:
		{
			if (ori == HORIZ)
			{

				if (m_vVelocity.x > 0.0f)
				{

				}
				else
				{

				}

				Vector2D v(m_vVelocity);
				v.Normalize();

			}
			else
			{

				if (m_vVelocity.y > 0.0f)
				{

				}
				else
				{

				}

				Vector2D v(m_vVelocity);
				v.Normalize();

			}
			break;
		}

		case ENTITY_CHARACTER:
		case ENTITY_PROJECTILE:
		case ENTITY_TRIGGER:
		case ENTITY_NONE:
		default:
			assert(false);
	}
}

