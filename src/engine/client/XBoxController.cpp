#include "XBoxController.h"

XBoxController::XBoxController(InputManager& rInputMgr) : GamePadController(rInputMgr)


{
   m_iXAxisMoveStick = 0;
   m_iYAxisMoveStick = 1;
   m_iXAxisWeaponStick = 4;
   m_iYAxisWeaponStick = 3;
   m_iPrevWeaponButton = 4;
   m_iNextWeaponButton = 5;
   m_iJumpButton = 2;
   m_iShootButton = 2;
}

XBoxController::~XBoxController() {}


