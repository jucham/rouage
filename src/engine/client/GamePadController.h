#ifndef GAMEPADCONTROLLER_H
#define GAMEPADCONTROLLER_H

#include "GameController.h"
#include <SDL2/SDL.h>

class GamePadController : public GameController
{
   public:
      GamePadController(InputManager& rInputMgr);
      virtual ~GamePadController();

      void close();

      bool isLeftMoveActived() const;
      bool isRightMoveActived() const;
      bool isJumpActived() const;
      int  XWeaponPointerPos();
      int  YWeaponPointerPos();
      bool isPrevWeaponButtonActived();
      bool isNextWeaponButtonActived();
      bool isShootButtonActived() const;

   protected:
      SDL_Joystick* m_pStick;

      int m_iXAxisMoveStick;
      int m_iYAxisMoveStick;

      int m_iXAxisWeaponStick;
      int m_iYAxisWeaponStick;
      int m_iLastXWeaponPointerPos;
      int m_iLastYWeaponPointerPos;
      int m_iPrevWeaponButton;
      int m_iNextWeaponButton;

      int m_iJumpButton;
      int m_iShootButton;

};

#endif // GAMEPADCONTROLLER_H
