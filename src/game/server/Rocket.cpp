#include "Rocket.h"
#include <game/server/GameServer.h>
#include "EntityEnumerations.h"
#include "Tile.h"

float Rocket::m_fWIDTH;
float Rocket::m_fHEIGHT;

Rocket::Rocket(Character* pShooter, const Vector2D& facing) :
      Projectile(pShooter, facing)
{
   assert(pShooter != 0);

   m_HitBox.setWidth(m_fWIDTH);
   m_HitBox.setHeight(m_fHEIGHT);
   m_pCurrentTileSpot = CurrentTileSpot();
   m_iDamage = 50;
   m_fG = Tile::m_fHEIGHT * 0.1f;
   m_fMagnitudeFactor = 0.5f;
   setImpulseForce();
}

Rocket::~Rocket()
{
	GameServer::Event event;
	event.type = EVENT_ROCKET_EXPLODE;
	event.pos = m_vPosition;
	m_pGameServer->AddEvent(event);
}

void Rocket::updateSpecial()
{

}

void Rocket::onCollide(EntityType entType, OrientHV ori)
{
}
