#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include <vector>
#include <GL/glew.h>
#include <base/Vector2D.h>
#include <base/Time.h>
#include "VBOSprite.h"
#include "GFXAsset.h"

class SpriteAnimation
{
   public:
      SpriteAnimation(const GFXAsset &gfxAsset,
                        int spriteDisplaytime,
                        VBOSprite *pSprite,
                        int spritesMappingIndex);

      int Play();

   private:
      int m_SpriteDisplayTime; // in milliseconds
      int m_TimeNextSprite;
      int m_CurSpriteIdx;
      VBOSprite *m_pSprite;
      std::vector< std::vector<float> > m_SpritesMapping;
};

#endif // SPRITEANIMATION_H
