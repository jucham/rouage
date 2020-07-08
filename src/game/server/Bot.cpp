#include "Bot.h"
#include <base/debug.h>
#include "WeaponSystem.h"
#include "BotWeaponSystem.h"
#include "ai/GoalThink.h"
#include "GameServer.h"
#include "Tile.h"
#include "Map.h"
#include "EntityEnumerations.h"
#include "ai/Waypoint.h"
#include "ai/SensoryMemory.h"
#include "ai/TargetingSystem.h"
#include "ai/NavGraphEdge.h"


Bot::Bot(GameServer* pGame, std::string name) :
      Character(pGame, name, false),

      m_bIsDoingLeftMove(false),
      m_bIsDoingRightMove(false),
      m_bIsDoingJump(false),
      m_bIsWalkingLeft(false),
      m_vLastTimePosition(0, 0),
      m_iTimeBetweenPosRecord(1000),
      m_iRecordLastTime(0),
      m_pTargSys(0),
      m_pSensMem(0),
      m_iLastWaypointToReachIdx(-1),
      m_pCurPathEdge(0),
      m_iBlockingState(NO_BLOCKED),
      //m_firstOldPosSetting(true),
      m_bTryToUnlock(false),
      //m_bIsStopped(true)
      m_iXMoveState(NO_MOVE),
      m_iOldPosEquaCounter(0),
      m_updateRegulator(100, MS_VALUE)
{
   m_bIsPlayer = false;

   // msec for reaction time
   // aim accuracy is max (0.0f)
   // msec for aim persistance
   m_pWeaponSys = new BotWeaponSystem(m_pGameServer, this, 100, 0.0f, 2000);

   m_pWeaponSys->addWeapon(WP_PISTOL);
   m_pWeaponSys->addWeapon(WP_ROCKET_LAUNCHER);
   m_pWeaponSys->addWeapon(WP_AK47);

   m_pWeaponSys->SelectWeapon(WP_AK47);

   m_pTargSys = new TargetingSystem(this);
   m_pSensMem = new SensoryMemory(this, 1000); // memory span is 5 seconds

   // create brain of bot, higher level goal which controls his behavior
   m_pBrain = new GoalThink(this);

   // some physic settings
   m_SideVelocity *= 0.7f;
   m_InitJumpVelocity *= 1.0f;
   m_fG *= 1.1f;

}

Bot::~Bot()
{
   //FIXME that delete causes crash
//   delete m_pWeaponSys;
   delete m_pSensMem;
   delete m_pBrain;
}

void Bot::updateSpecial()
{

#if DBG
   dbg << "### Bot " << m_sName << "###" << std::endl;

   dbg << "# Process()" << std::endl;
#endif

   // runs current goal and its subgoals
   m_pBrain->Process();

#if DBG
   dbg << " BEFORE -> P0 : " << m_vPosition.x << " P1 : " << m_vOldPosition.x << " P2 : " << m_vOldPos2.x << std::endl;
   dbg << "# updateMovement()" << std::endl;
#endif
   updateMovement();

#if DBG
   dbg << " AFTER -> P0 : " << m_vPosition.x << " P1 : " << m_vOldPosition.x << " P2 : " << m_vOldPos2.x << std::endl;
   dbg << "MOVE " << ((m_iXMoveState == NO_MOVE) ? "OFF" : "ON") << std::endl;
#endif

#if DBG
   dbg << "# updateBlockingState()" << std::endl;
#endif
   updateBlockingState();

#if DBG
   //dbg << "@ OLD pos equal counter = " << m_iOldPosEquaCounter << std::endl;
#endif

//if ( m_updateRegulator.isReady() )
//{
   m_pTargSys->Update();
   m_pSensMem->UpdateVision();
//}

#if DBG
   dbg << "# Arbitrate()" << std::endl;
#endif
   // decides which goal is most desirable
   m_pBrain->Arbitrate();

   //this method aims the bot's current weapon at the current target
   //and takes a shot if a shot is possible
   //m_pWeaponSys->TakeAimAndShoot();

   m_pWeaponSys->Update();

#if DBG
   dbg << std::endl;
#endif

}

GoalThink* const Bot::getBrain()
{
   return m_pBrain;
}

void Bot::updateBlockingState()
{
   if (m_iBlockingState != NO_BLOCKED) return;

   if (!isMoveOff() && m_vOldPosition == m_vOldPos2)
   {
      m_iOldPosEquaCounter++;
#if DBG
      dbg << " -> " << m_iOldPosEquaCounter << " x OLD POS EQUAL " << std::endl;
#endif
   }

   // blocked by stair, or other obstacle
   //if ( !isMoveOff() && m_vOldPosition == m_vOldPos2 )
   if (!isMoveOff() && m_iOldPosEquaCounter > 10)
   {
      m_iOldPosEquaCounter = 0;
      m_iBlockingState = BLOCKED_CAN_UNLOCK;

#if DBG
      dbg << "BLOCKED_CAN_UNLOCK" << std::endl;
#endif
   }
   else
   {
      m_iBlockingState = NO_BLOCKED;
   }

}

bool Bot::isTouchingWaypoint(Waypoint& rWpt)
{
   return BBox()->isOverlappedWith(AABBox2D(rWpt.Position(), Tile::m_fWIDTH / 2, Tile::m_fHEIGHT / 2));
}

void Bot::putInRightDirection(Waypoint& rWpt)
{
   if (!isInRightDirection(rWpt))
   {
      if (!canReachWaypoint(rWpt))
      {
#if DBG
         dbg << "CANT REACH WAYPOINT N° " << rWpt.Index() << std::endl;
         dbg << "BLOCKED_CAN_NOT_UNLOCK" << std::endl;
#endif
         m_iBlockingState = BLOCKED_CAN_NOT_UNLOCK;
         return;
      }
   }

   // reset moving
   //moveOff();

   // and then puts in good direction
   if ((rWpt.Position().x < m_vPosition.x))
   {
      doingLeftMove_On();
   }
   else
   {
      doingRightMove_On();
   }
}

bool Bot::isInRightDirection(Waypoint& rWpt) const
{
   return ((rWpt.Position().x < m_vPosition.x) && isDoingLeftMove()) || ((rWpt.Position().x > m_vPosition.x) && isDoingRightMove());
}

const TargetingSystem* const Bot::TargetSys() const
{
   return m_pTargSys;
}
Character* Bot::TargetBot() const
{
   return m_pTargSys->GetTarget();
}
SensoryMemory* const Bot::SensoryMem() const
{
   return m_pSensMem;
}

void Bot::updateMovement()
{
   UpdatePos(GetGameServer()->GetTimer());

   //if bot jumps then he doesn't jump until that a new order given him
   if (m_Jumping)
   {
      m_bIsDoingJump = false;
   }

}

void Bot::setLastWaypointToReach(int idx)
{
   m_iLastWaypointToReachIdx = idx;
}

void Bot::setCurrentPathEdge(NavGraphEdge* pEdge)
{
   m_pCurPathEdge = pEdge;
}

int Bot::BlockingState()
{
   return m_iBlockingState;
}

bool Bot::isBlocked() const
{

   return false;
}

void Bot::setNoBlocked()
{
   m_iBlockingState = NO_BLOCKED;
}

void Bot::oneTryToUnlock()
{
   m_bTryToUnlock = true;
}

bool Bot::triedToUnlock() const
{
   return m_bTryToUnlock;
}

void Bot::resetTryToUnlock()
{
   m_bTryToUnlock = false;
}

bool Bot::canReachWaypoint(Waypoint& rWpt)
{
   int ibreak = 0;

   std::vector < std::vector<Tile*> > tiles = m_pGameServer->GetMap()->getTiles();

   int txChar = Tile::tileContainsCenterColumn(m_vPosition.x + Character::m_fWIDTH / 2);
   int tyChar = Tile::tileContainsCenterLine(m_vPosition.y + Character::m_fHEIGHT / 2);

   int tx = txChar;
   int ty = tyChar;

   int txWpt = Tile::tileContainsCenterColumn(rWpt.Position().x);
   int tyWpt = Tile::tileContainsCenterLine(rWpt.Position().y);

   int deltaX = 0;
   if (txChar - txWpt < 0)
   {
      deltaX = 1;
   }
   else if (txChar - txWpt > 0)
   {
      deltaX = -1;
   }

   int deltaY = 0;
   if (tyChar - tyWpt < 0)
   {
      deltaY = 1;
   }
   else if (tyChar - tyWpt > 0)
   {
      deltaY = -1;
   }

   int oldTx = 0;
   int oldTy = 0;

   while (1)
   {
      if (tx == txWpt && ty == tyWpt)
      {
         return true;
      } // waypoint reached !

      bool xWptReached = false;
      if (deltaX > 0)
      {
         if (tx >= txWpt)
         {
            xWptReached = true;
         }
      }
      else
      {
         if (tx <= txWpt)
         {
            xWptReached = true;
         }
      }

      // next tile on x is not solid
      // and waypoint not reached on x, so does a step on x
      if (!tiles[tx + deltaX][ty]->isSolid() && !xWptReached) tx += deltaX;

      // next tile on y is not solid, does a step on y
      else if (!tiles[tx][ty + deltaY]->isSolid()) ty += deltaY;

      // collision with a wall, so fail
      else return false;

      // particular fail
      if (tx == oldTx && ty == oldTy) return false;

      oldTx = tx;
      oldTy = ty;
   }
}

void Bot::walkInOppositeWay()
{

   if (isDoingLeftMove())
   {
      doingRightMove_On();
   }
   else if (isDoingRightMove())
   {
      doingLeftMove_On();
   }

   // bot don't move so randomly choose left or right side
   else
   {
      (RandInt(0, 1) == 0) ? doingLeftMove_On() : doingRightMove_On();
   }
}
