#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "InputManager.h"

class GameController
{
   public:
      GameController(InputManager& rInputMgr);
      virtual ~GameController();

      virtual void close() = 0;
      virtual bool isLeftMoveActived() const = 0;
      virtual bool isRightMoveActived() const = 0;
      virtual bool isJumpActived() const = 0;
      virtual int  XWeaponPointerPos() = 0;
      virtual int  YWeaponPointerPos() = 0;
      virtual bool isPrevWeaponButtonActived() = 0;
      virtual bool isNextWeaponButtonActived() = 0;
      virtual bool isShootButtonActived() const = 0;

      void collectInput();

   protected:
      InputManager& m_rInputMgr;
};

#endif // GAMECONTROLLER_H
