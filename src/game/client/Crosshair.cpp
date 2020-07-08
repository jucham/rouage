#include "Crosshair.h"
#include <game/client/GameClient.h>
#include "Tile.h"
#include "WeaponSystem.h"

Crosshair::Crosshair(Game* pGame) :
   m_pGame(pGame),
   m_sprite(pGame->getGfxAsset(),
                           TEX_CROSSHAIR,
                           GL_STREAM_DRAW,
                           GL_STATIC_DRAW,
                           GL_STATIC_DRAW,
                           Tile::m_fWIDTH,
                           Tile::m_fWIDTH)
{
   m_sprite.SetPosition(0,0);
   m_sprite.setTexMapping();
   m_sprite.setCentered();
   m_sprite.setColor(1.0f, 1.0f, 1.0f, 0.6f);
}

void Crosshair::update()
{
   Character* pPlayer = m_pGame->getPlayer();
   Vector2D weaponPos( pPlayer->WeaponSys()->WeaponPos() );
   Vector2D mousePos ( m_pGame->Cam()->GetPos().x +  m_pGame->InputMgr().MouseX(),
                      m_pGame->Cam()->GetPos().y + m_pGame->RenderMgr()->FrameHeight() - m_pGame->InputMgr().MouseY());

   // vector from center of character to mouse pointer position
   Vector2D charToMouse( mousePos - weaponPos );

   // crosshair goes too far, so replace to max distance
   float maxDist = m_pGame->RenderMgr()->FrameWidth()/4;
   if( weaponPos.Distance(mousePos) > maxDist )
   {
      charToMouse.Normalize();
      charToMouse *= maxDist;
   }

   m_sprite.SetPosition( weaponPos.x + charToMouse.x,
                        weaponPos.y + charToMouse.y );
}

void Crosshair::render()
{
   m_sprite.Render();
}

