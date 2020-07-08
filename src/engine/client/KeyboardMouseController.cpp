#include "KeyboardMouseController.h"


KeyboardMouseController::KeyboardMouseController(InputManager& rInputMgr) : GameController(rInputMgr) {}

void KeyboardMouseController::close() {}

bool KeyboardMouseController::isLeftMoveActived() const
{
   return m_rInputMgr.IsKeyPressed(SDL_SCANCODE_LEFT) || m_rInputMgr.IsKeyPressed(SDL_SCANCODE_a);
}

bool KeyboardMouseController::isRightMoveActived() const
{
   return m_rInputMgr.IsKeyPressed(SDL_SCANCODE_RIGHT) || m_rInputMgr.IsKeyPressed(SDL_SCANCODE_d);
}

bool KeyboardMouseController::isJumpActived() const
{
   return m_rInputMgr.IsKeyPressed(SDL_SCANCODE_SPACE);
}

int  KeyboardMouseController::XWeaponPointerPos()
{
   return m_rInputMgr.MouseX();
}

int  KeyboardMouseController::YWeaponPointerPos()
{
   return m_rInputMgr.MouseY();
}

bool KeyboardMouseController::isPrevWeaponButtonActived()
{
   bool actived = false;
   if (m_rInputMgr.IsMouseButtonPressed(SDL_BUTTON_WHEELUP))
   {
      actived = true;
      m_rInputMgr.SetMouseButtonState(SDL_BUTTON_WHEELUP , false);
   }

   return actived;
}

bool KeyboardMouseController::isNextWeaponButtonActived()
{
  bool actived = false;
   if (m_rInputMgr.IsMouseButtonPressed(SDL_BUTTON_WHEELDOWN))
   {
      actived = true;
      m_rInputMgr.SetMouseButtonState(SDL_BUTTON_WHEELDOWN, false);
   }

   return actived;
}

bool KeyboardMouseController::isShootButtonActived() const
{
   return m_rInputMgr.IsMouseButtonPressed(SDL_BUTTON_LEFT);
}
