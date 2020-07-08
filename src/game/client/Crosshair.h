#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "../gfx/VBOSprite.h"

class Game;

class Crosshair
{
   public:
      Crosshair(Game* pGame);
      void update();
      void render();

   protected:
   Game*    m_pGame;
   VBOSprite   m_sprite;

};

#endif // CROSSHAIR_H
