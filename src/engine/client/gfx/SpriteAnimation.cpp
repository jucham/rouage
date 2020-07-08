#include "SpriteAnimation.h"
#include <game/server/Character.h>
#include <base/debug.h>

SpriteAnimation::SpriteAnimation(const GFXAsset &gfxAsset,
                                 int spriteDisplaytime,
                                 VBOSprite *pSprite,
                                 int spritesMappingIndex) :
   m_SpriteDisplayTime(spriteDisplaytime),
   m_TimeNextSprite(0),
   m_CurSpriteIdx(0),
   m_pSprite(pSprite),
   m_SpritesMapping(gfxAsset.getTexMapping(spritesMappingIndex))
{

}


int SpriteAnimation::Play()
{
   // when time between sprite is 0 then that means animation
   // with only one sprite then don't change texture mapping
   if (m_SpriteDisplayTime > 0)
   {
      int curTime = Time::ms();

      if (curTime >= m_TimeNextSprite)
      {
         m_TimeNextSprite = curTime + m_SpriteDisplayTime;
         m_CurSpriteIdx = (m_CurSpriteIdx+1) % m_SpritesMapping.size();
      }
   }

   m_pSprite->setTexMapping (m_SpritesMapping[ m_CurSpriteIdx ][0],
                          m_SpritesMapping[ m_CurSpriteIdx ][1],
                          m_SpritesMapping[ m_CurSpriteIdx ][2],
                          m_SpritesMapping[ m_CurSpriteIdx ][3]);
   return m_CurSpriteIdx;
}
