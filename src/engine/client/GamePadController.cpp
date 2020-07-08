#include "GamePadController.h"
#include <cassert>

#include "debug.h"

GamePadController::GamePadController(InputManager& rInputMgr) : GameController(rInputMgr),
   m_pStick(0),
   m_iXAxisMoveStick(0),
   m_iYAxisMoveStick(0),
   m_iXAxisWeaponStick(0),
   m_iYAxisWeaponStick(0),
   m_iPrevWeaponButton(0),
   m_iNextWeaponButton(0),
   m_iJumpButton(0),
   m_iShootButton(0)

{
   SDL_JoystickEventState(SDL_ENABLE);
   m_pStick = SDL_JoystickOpen(0);
   assert (m_pStick != 0);
}


GamePadController::~GamePadController()
{
    //FIXME: problem when close joystick
//   if(SDL_JoystickOpened(0))
//   {
//      dbg << "JOY CLOSED" << std::endl;
//      SDL_JoystickClose(m_pStick);
//   }
}

void GamePadController::close()
{

}


bool GamePadController::isLeftMoveActived() const
{
   return m_rInputMgr.GetAxisValues( m_iXAxisMoveStick ) <= -8000;
}

bool GamePadController::isRightMoveActived() const
{
   return m_rInputMgr.GetAxisValues( m_iXAxisMoveStick ) >= 8000;
}

bool GamePadController::isJumpActived() const
{
   return m_rInputMgr.GetAxisValues( m_iJumpButton ) >= 10000;
}

int  GamePadController::XWeaponPointerPos()
{
   int value = m_rInputMgr.GetAxisValues( m_iXAxisWeaponStick);

   // value is good
   if ((value<=-6000) || (value>=6000))
   {
      m_iLastXWeaponPointerPos = value;
      return value;
   }

   // else return old value
   return m_iLastXWeaponPointerPos;
}

int  GamePadController::YWeaponPointerPos()
{
   int value = m_rInputMgr.GetAxisValues( m_iYAxisWeaponStick);

   // value is good
   if ((value<=-6000) || (value>=6000))
   {
      m_iLastYWeaponPointerPos = value;
      return value;
   }

   // else return old value
   return m_iLastYWeaponPointerPos;
}

bool GamePadController::isPrevWeaponButtonActived()
{
	return m_rInputMgr.IsJoystickButtonPressed( m_iPrevWeaponButton );
}

bool GamePadController::isNextWeaponButtonActived()
{
	return m_rInputMgr.IsJoystickButtonPressed( m_iNextWeaponButton );
}

bool GamePadController::isShootButtonActived() const
{
   return m_rInputMgr.GetAxisValues( m_iShootButton ) <= -10000;
}

