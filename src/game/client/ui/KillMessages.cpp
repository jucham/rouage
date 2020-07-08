#include "KillMessages.h"

#include <base/Time.h>
#include "../base/Camera.h"
#include "../base/Window.h"
#include <game/server/Tile.h>
#include "../gfx/VBOSprite.h"
#include "../gfx/GFXAsset.h"
#include "TextRender.h"


KillMessages::KillMessages(Camera& rCam,
                           GFXAsset& rGfxAsset,
                           Window& rRenderMgr,
                           TextRender& rTextMgr) :
   m_msgs(),
   m_vOrigin(),
   m_rCam(rCam),
   m_rGfxAsset(rGfxAsset),
   m_rRenderMgr(rRenderMgr),
   m_rTextRender(rTextMgr)
{

}

void KillMessages::addMessage(const std::string killerName,
                 const std::string deadName,
                 int weaponType)
{
   KillMsg km;

   km.killerName = killerName;
   km.deadName = deadName;
   km.weaponType = weaponType;
   km.startTime = Time::ms();

   km.pSprite = 0;
   VBOSprite* pSpr = new VBOSprite( m_rGfxAsset,
                TEX_WEAPONS,
                GL_STREAM_DRAW,
                GL_STATIC_DRAW,
                GL_STATIC_DRAW,
                0,
                0);

   Float2DArray mapping = m_rGfxAsset.getTexMapping( MAP_WEAPON_SET );
   pSpr->setTexMapping( mapping[weaponType] );

   // take relative dimensions of pistol to compute others
   float tsize = Tile::m_fWIDTH;
   Vector2D wpSize (tsize, tsize/2);
   wpSize *= 1.2f;

   switch(weaponType)
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

   pSpr->setSize(wpSize);
   pSpr->SetOffset(-1/2.0f * wpSize);
   pSpr->SetVisible();

   km.pSprite = pSpr;


   if ( m_msgs.size() >= 5 )
   {
      m_msgs.pop_front();
   }

   m_msgs.push_back(km);
}

void KillMessages::update()
{
   m_vOrigin.x = m_rCam.GetPos().x + m_rRenderMgr.FrameWidth();
   m_vOrigin.y = m_rCam.GetPos().y + m_rRenderMgr.FrameHeight();

   std::list<KillMsg>::iterator pCurKillMsg = m_msgs.begin();

   while ( pCurKillMsg != m_msgs.end() )
   {
      const KillMsg& msg = *pCurKillMsg;

      if ( Time::ms() - msg.startTime > 8000 )
      {
         delete (*pCurKillMsg).pSprite;
         pCurKillMsg = m_msgs.erase(pCurKillMsg);
      }
      else
      {
         ++pCurKillMsg;
      }
   }
}

void KillMessages::render()
{
   float tsize = Tile::m_fWIDTH;
   int   fontIdx = 1;
   float y = m_vOrigin.y;
   std::list<KillMsg>::const_iterator pCurKillMsg;



   for (  pCurKillMsg = m_msgs.begin();
            pCurKillMsg != m_msgs.end(); ++pCurKillMsg )
   {
      const KillMsg& msg = *pCurKillMsg;

      float killerNameWidth = static_cast<float> (m_rTextRender.Font(fontIdx).calcStringWidth( msg.killerName ));
      float deadNameWidth = static_cast<float> (m_rTextRender.Font(fontIdx).calcStringWidth( msg.deadName ));
      float spaceWidth =   tsize * 0.5f;
      float spriteWidth =   msg.pSprite->Width();

      float msgWidth = killerNameWidth + deadNameWidth + 2*spaceWidth + spriteWidth;

      float x = m_vOrigin.x - msgWidth;

      y -= tsize;

      m_rTextRender.drawText(x, y, msg.killerName, fontIdx);
      x = m_vOrigin.x - deadNameWidth;
      m_rTextRender.drawText(x, y, msg.deadName, fontIdx);

      glLoadIdentity();
      msg.pSprite->SetPosition(m_vOrigin.x - deadNameWidth - spaceWidth - msg.pSprite->Width()/2,
                               y);
      msg.pSprite->Render();
   }
}
