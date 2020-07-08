#include "Corpse.h"

#include <base/Time.h>
#include <game/server/GameServer.h>
#include "Character.h"
#include "Tile.h"
#include "../gfx/GFXAsset.h"

#include <base/debug.h>

Corpse::Corpse(GameServer* pGame, Vector2D pos, OrientTBLR ori, bool isPlayer) :
      m_bIsPlayer(isPlayer),
      m_bIsGibbed(false),
      m_iStartRotTime(Time::ms()),
      m_iRotTimeLength(250),
      m_fRotAngle(((ori == LEFT) ? -HalfPi : HalfPi)),
      m_iTranspaPeriod(1000)

{
   m_bbox.setWidth(Character::m_fWIDTH);
   m_bbox.setHeight(Character::m_fWIDTH);
   m_endColor[3] = 0.0f;
   m_bChangingColor = true;
}

void Corpse::update()
{
   updateVelocity();
   updatePosition();
}

void Corpse::onCollide(EntityType entType, OrientHV ori)
{
   switch (entType)
   {
   case ENTITY_TILE:
      m_vV0.Zero();
      m_iStartFlyingTime = Time::ms();
      break;

   case ENTITY_PROJECTILE:
      // if life < -20 then gib()

      break;

   default:
      break;
   }

}

void Corpse::updateSpecial()
{
}
