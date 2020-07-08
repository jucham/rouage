#ifndef KILLMESSAGES_H
#define KILLMESSAGES_H

#include <string>
#include <list>

#include <base/Vector2D.h>

class VBOSprite;
class Camera;
class GFXAsset;
class Window;
class TextRender;

struct KillMsg
{
   std::string    killerName;
   std::string    deadName;
   unsigned int   weaponType;
   int            startTime;
   VBOSprite*        pSprite;

};

class KillMessages
{
   public:
      KillMessages(Camera& rCam,
                   GFXAsset& rGfxAsset,
                   Window& rRenderMgr,
                   TextRender& rTextMgr);

      void addMessage(const std::string killerName,
                 const std::string deadName,
                 int weaponType);

      void update();
      void render();

   protected:
      std::list<KillMsg>   m_msgs;
      Vector2D             m_vOrigin;
      Camera&              m_rCam;
      GFXAsset&            m_rGfxAsset;
      Window&       m_rRenderMgr;
      TextRender&         m_rTextRender;

};

#endif // KILLMESSAGES_H
