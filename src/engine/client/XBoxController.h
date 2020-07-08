#ifndef XBOXCONTROLLER_H
#define XBOXCONTROLLER_H

#include "GamePadController.h"

class InputManager;

class XBoxController : public GamePadController
{
   public:
      XBoxController(InputManager& rInputMgr);
      ~XBoxController();

};

#endif // XBOXCONTROLLER_H
