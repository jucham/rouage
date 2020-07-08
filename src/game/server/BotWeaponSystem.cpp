#include "BotWeaponSystem.h"
#include "Character.h"
#include "EntityEnumerations.h"
#include "Bot.h"
#include  <game/server/ai/TargetingSystem.h>
#include "Weapon.h"
#include "Tile.h"
#include "GameServer.h"
#include <base/debug.h>
//#include "../ai/SensoryMemory.h"


BotWeaponSystem::BotWeaponSystem(GameServer* pGame,
                        Bot* pOwner,
                        int reactionTime,
                        float aimAccuracy,
                        int aimPersistance) : WeaponSystem(pGame),
      m_pOwner(pOwner),
      m_iReactionTime(reactionTime),
      m_fAimAccuracy(aimAccuracy),
      m_iAimPersistance(aimPersistance),
      m_fMaxTurnRate(0.3f)
{
   // sets weapon angle to 45 degree to top right
   assert ( pGame != 0 );
   assert ( pOwner != 0 );
   m_Direction = Vector2D(1,1);
}

BotWeaponSystem::~BotWeaponSystem() {}


void BotWeaponSystem::update()
{

   changeWeapon();

   // update position
   m_vPosition.x = m_pOwner->Position().x + Character::m_fWIDTH*0.5f;
   m_vPosition.y = m_pOwner->Position().y + Character::m_fHEIGHT*0.75f;


  if ( (m_pOwner->TargetSys()->GetTimeTargetHasBeenOutOfView() < m_iAimPersistance) )
  {
		//the position the weapon will be aimed at
		Vector2D AimingPos( m_pOwner->TargetBot()->Position());

		if (  RotateFacingTowardPosition(AimingPos, false)
            && m_pOwner->TargetSys()->isTargetShootable()
            && (m_pOwner->TargetSys()->GetTimeTargetHasBeenVisible() > m_iReactionTime) )
		{
			//AddNoiseToAim(AimingPos);
			//GetCurrentWeapon()->ShootAt(AimingPos);
			if( m_pCurrentWeapon->Ready() )
			{
				m_pCurrentWeapon->DoFire();
			}
		}
  }
  else
   {
      //no target to shoot at so rotate facing to be in horizontal position
      Vector2D horizAimPos;
      horizAimPos.x = (m_pOwner->isDoingLeftMove()) ? -100.0f : 100.0f;
      horizAimPos.y = 0.0f;
      RotateFacingTowardPosition( m_vPosition + horizAimPos, false);
   }

}


bool BotWeaponSystem::changeWeapon()
{
   int oldWpType = m_pCurrentWeapon->getType();
   int newWpType = WP_PISTOL;

   // if current weapon void so we take the most full
   if (m_pCurrentWeapon->NoAmmo())
   {
      int bestWeaponAmmoCount = 0;

      WeaponMap::const_iterator pCurWeapon;
      for(pCurWeapon = m_Weapons.begin(); pCurWeapon != m_Weapons.end(); ++pCurWeapon)
      {
         int curCount = ((*pCurWeapon).second)->AmmoCount();

         if( curCount >  bestWeaponAmmoCount)
         {
            bestWeaponAmmoCount = curCount;
            newWpType = (*pCurWeapon).first;
         }
      }

      if (newWpType != oldWpType)
      {
         SelectWeapon(newWpType);
         return true;
      }
   }

   return false;
}

void BotWeaponSystem::updateFacing()
{
   // RotateFacingTowardPosition()
}

bool BotWeaponSystem::isShooting()
{
   return false;
}

//------------------ RotateFacingTowardPosition -------------------------------
//
//  given a target position, this method rotates the bot's facing vector
//  by an amount not greater than m_dMaxTurnRate until it
//  directly faces the target.
//
//  returns true when the heading is facing in the desired direction
//----------------------------------------------------------------------------
bool BotWeaponSystem::RotateFacingTowardPosition(const Vector2D& target, bool fullRotation)
{
   Vector2D targCenter = target + Vector2D(Character::m_fWIDTH/2, Character::m_fHEIGHT/2);
   Vector2D toTarget = Vec2DNormalize( targCenter - m_vPosition );

   m_Direction.Normalize();
   float dot = m_Direction.Dot(toTarget);

   //clamp to rectify any rounding errors
   Clamp(dot, -1, 1);

   //determine the angle between the heading vector and the target
   float angle = std::acos(dot); // angle en radians

   //return true if the bot's facing is within WeaponAimTolerance degs of
   //facing the target
   const float WeaponAimTolerance = 0.002f; // 0.001 ~= 2 degs approx

   if (angle < WeaponAimTolerance)
   {
      m_Direction = toTarget;
      computeWeaponAngle();
      return true;
   }

   //clamp the amount to turn to the max turn rate
   if (angle > m_fMaxTurnRate) angle = m_fMaxTurnRate;

   angle *= m_Direction.Sign(toTarget);

   float cs = std::cos(angle);
   float sn = std::sin(angle);

   float fx = m_Direction.x * cs - m_Direction.y * sn;
   float fy = m_Direction.x * sn + m_Direction.y * cs;

   m_Direction.x = fx;
   m_Direction.y = fy;

   computeWeaponAngle();

   return false;
}

Character* BotWeaponSystem::Shooter() const
{
   return m_pOwner;
}
