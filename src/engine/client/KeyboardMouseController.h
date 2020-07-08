#ifndef KEYBOARDMOUSECONTROLLER_H
#define KEYBOARDMOUSECONTROLLER_H

#include "GameController.h"

class KeyboardMouseController : public GameController
{
   public:
      KeyboardMouseController(InputManager& rInputMgr);

      void close();
      bool isLeftMoveActived() const;
      bool isRightMoveActived() const;
      bool isJumpActived() const;
      int  XWeaponPointerPos();
      int  YWeaponPointerPos();
      bool isPrevWeaponButtonActived();
      bool isNextWeaponButtonActived();
      bool isShootButtonActived() const;
};

#endif // KEYBOARDMOUSECONTROLLER_H
