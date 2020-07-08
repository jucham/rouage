#ifndef HUD_H
#define HUD_H

#include <base/Vector2D.h>
#include "../gfx/VBOSprite.h"
#include "Panel.h"

class Game;
class TextRender;
class TextString;
class Character;

class HUD
{
   public:
      HUD(Game* pGame, TextRender& rTextMgr);

      void render();
      void renderHealthAndWeapon();
      void renderAmmo();
      void renderScoreBoard();

   protected:
      Game*          m_pGame;
      Vector2D       m_vOrigin;
      Character*     m_pPlayer;
      TextRender&   m_rTextMgr;

      VBOSprite         m_sprHealth;
      VBOSprite         m_sprWeapon;

      Panel m_healthPanel;
      Panel m_weaponPanel;
      Panel m_bgScorePanel;
      Panel m_highlightPanel;

};

#endif // HUD_H
