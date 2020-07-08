#include "HUD.h"

#include <sstream>

#include <game/client/GameClient.h>
#include <game/server/Character.h>
#include "../base/Camera.h"
#include <game/server/Tile.h>
#include "../base/Weapon.h"
#include <game/server/WeaponSystem.h>
#include "../base/Window.h"
#include "../base/InputManager.h"
#include <base/debug.h>
#include <base/Vector2D.h>
#include "../gfx/GFXAsset.h"
#include "TextString.h"


HUD::HUD(Game* pGame, TextRender& rTextMgr) :
   m_pGame(pGame),
   m_vOrigin(),
   m_pPlayer(pGame->getPlayer()),
   m_rTextMgr(rTextMgr),

   m_sprHealth( m_pGame->getGfxAsset(),
                TEX_HUD,
                GL_STREAM_DRAW,
                GL_STATIC_DRAW,
                GL_STATIC_DRAW,
                Tile::m_fWIDTH,
                Tile::m_fWIDTH),

   m_sprWeapon( m_pGame->getGfxAsset(),
                TEX_WEAPONS,
                GL_STREAM_DRAW,
                GL_STREAM_DRAW,
                GL_STATIC_DRAW,
                Tile::m_fWIDTH,
                Tile::m_fWIDTH),

   m_healthPanel( 3.5*Tile::m_fWIDTH, 2*Tile::m_fWIDTH, 15, 15),
   m_weaponPanel( 6*Tile::m_fWIDTH, 2*Tile::m_fWIDTH, 15, 15),
   m_bgScorePanel( 20*Tile::m_fWIDTH, 15*Tile::m_fHEIGHT, 15, 15),
   m_highlightPanel( 19*Tile::m_fWIDTH, 2*Tile::m_fHEIGHT, 10, 10)
{

   Float2DArray mapping = m_pGame->getGfxAsset().getTexMapping( MAP_HUD_HEALTH );
   m_sprHealth.setTexMapping( mapping[0] );
   m_sprHealth.setCentered();
   m_sprHealth.SetVisible();

   m_sprWeapon.setCentered();
   m_sprWeapon.SetVisible();
}


void HUD::render()
{

   m_vOrigin = m_pGame->Cam()->GetPos();

   if ( m_pPlayer->isAlive() )
   {
      renderHealthAndWeapon();
   }

   if (m_pGame->InputMgr().IsKeyPressed(SDLK_TAB) )
   {
      renderScoreBoard();
   }



}

void HUD::renderHealthAndWeapon()
{
   float tsize = Tile::m_fWIDTH;
   float x     = m_vOrigin.x + tsize;
   float y     = m_vOrigin.y + 2*tsize;

   // health panel
   m_healthPanel.Render( x-tsize/2, y-tsize, 0.26f,0.18f,0.12f, 0.3f);

   // Health sprite
   m_sprHealth.SetPosition( x+tsize/2, y );
   m_sprHealth.Render();
   x += 1.5*tsize;

   // Health counter
   std::ostringstream oss;
   oss << m_pPlayer->GetLife();
   std::string text (oss.str());


   glEnable(GL_TEXTURE_2D);
   m_rTextMgr.drawText( x , y, text, 3);
   glDisable(GL_TEXTURE_2D);

   // weapon panel
   x += 3.5*tsize;
   m_weaponPanel.Render(x-tsize/2, y-tsize, 0.26f,0.18f,0.12f, 0.3f);

   // weapon sprite
   x += 1.5*tsize;
   m_sprWeapon.SetPosition(x, y);

   Weapon*        pWp = m_pPlayer->WeaponSys()->CurrentWeapon();
   unsigned int   wpType = pWp->getType();

   Float2DArray mapping = m_pGame->getGfxAsset().getTexMapping( MAP_WEAPON_SET );
   m_sprWeapon.setTexMapping( mapping[wpType] );

   // take relative dimensions of pistol to compute others
   Vector2D wpSize (tsize, tsize/2);
   wpSize *= 1.5f;

   switch(wpType)
   {
      case WP_PISTOL:
         break;

      case WP_ROCKET_LAUNCHER:
         wpSize.x *= 1.5f;
         wpSize.y *= 1.5f;
         break;

      case WP_AK47:
         wpSize.x *= 1.8f;
         break;

      case WP_SHOTGUN:
         wpSize.x *= 1.9f;
         break;

      default:
         break;
   }

   m_sprWeapon.setSize(wpSize);
   m_sprWeapon.Render();

   // weapon counter
   x += 2*tsize;
   oss.str("");
   oss << pWp->AmmoCount();
   text = oss.str();
   glEnable(GL_TEXTURE_2D);
   m_rTextMgr.drawText( x , y, text, 3);
   glDisable(GL_TEXTURE_2D);

}

void HUD::renderScoreBoard()
{
   float tsize = Tile::m_fWIDTH;
   std::ostringstream oss;
   std::string text;
   const std::vector<CharScore>& scores = m_pGame->GetScoring().Scores();


   // origin of score board
   float x = m_vOrigin.x + m_pGame->RenderMgr()->FrameWidth()/2 - m_bgScorePanel.Width()/2;
   float y = m_vOrigin.y + m_pGame->RenderMgr()->FrameHeight()/2 - m_bgScorePanel.Height()/2;

   m_bgScorePanel.Render(x, y, 00.26f,0.18f,0.12f, 0.3f);

   // Score board title
   x += tsize;
   y += m_bgScorePanel.Height() - tsize;
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   m_rTextMgr.drawText( x , y, "Score Board", 6);

   // Score of player
   oss << scores[0].kill;
   text = oss.str();
   m_rTextMgr.drawText( x + m_bgScorePanel.Width() - 3*tsize, y, text, 6);

   // Score board column titles
   float xName = x;
   float xKill = xName + 10*tsize;
   float xDeath = xKill + 3*tsize;

   y -= 1.5*tsize;
   m_rTextMgr.drawText( xName, y, "Name", 3);
   m_rTextMgr.drawText( xKill, y, "Kill", 3);
   m_rTextMgr.drawText( xDeath, y, "Death", 3);

   // All scores
   for(int i=0; i<scores.size(); i++)
   {

      y -= 2*tsize;

      if(i==0)
      {
         m_highlightPanel.Render(x-tsize/2, y-tsize, 0.39f, 0.26f, 0.17f, 0.5f);
      }

      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

      oss.str("");
      oss << scores[i].name;
      text = oss.str();
      m_rTextMgr.drawText( xName, y, text, 2);

      oss.str("");
      oss << scores[i].kill;
      text = oss.str();
      m_rTextMgr.drawText( xKill, y, text, 2);

      oss.str("");
      oss << scores[i].death;
      text = oss.str();
      m_rTextMgr.drawText( xDeath, y, text, 2);

   }
   glLoadIdentity();

}
