
#ifndef BOT_H
#define BOT_H

#include "Character.h"
#include <list>
#include <base/Regulator.h>

class GoalThink;
class Waypoint;
class SensoryMemory;
class TargetingSystem;
class NavGraphEdge;

enum
{
   NO_BLOCKED,
   BLOCKED_CAN_UNLOCK,
   BLOCKED_CAN_NOT_UNLOCK
};

/*!
*  \class Bot Bot.h
*  \brief This class implements a bot character
*/
class Bot : public Character
{
   protected:

      bool m_bIsDoingLeftMove;   ///< indicate if bot is moving to left
      bool m_bIsDoingRightMove;  ///< indicate if bot moving to right
      bool m_bIsDoingJump;       ///< indicate if bot is jumping
      bool m_bIsWalkingLeft;     ///< indicate if bot is walking to left

      GoalThink*  m_pBrain; ///< this object handles the arbitration and processing of high level goals

      Vector2D m_vLastTimePosition;
      int m_iTimeBetweenPosRecord;
      int m_iRecordLastTime;

      TargetingSystem*  m_pTargSys;
      SensoryMemory*    m_pSensMem;

      int m_iLastWaypointToReachIdx;
      NavGraphEdge* m_pCurPathEdge;

      int   m_iBlockingState;
      //bool  m_firstOldPosSetting;
      bool  m_bTryToUnlock;

      int m_iOldPosEquaCounter;

      Regulator m_updateRegulator;

      //bool m_bIsStopped;

      enum
      {
         MOVE_LEFT,
         MOVE_RIGHT,
         NO_MOVE
      };

      char m_iXMoveState;

   public:


      /*!
      *  \brief Constructor
      *  \param pGame pointer to game
      *  \param name name of new bot
      */
      Bot(GameServer* pGame, std::string name);
      /*!
      *  \brief Destructor
      */
      ~Bot();

      /*!
      *  \fn bool isDoingLeftMove()
      *  \brief Returns true if bot decides to forward to left
      *  \return true if bot decides to turn to left
      */
      //bool isDoingLeftMove() const { return m_bIsDoingLeftMove;}
      bool isDoingLeftMove() const { return m_iXMoveState == MOVE_LEFT;}

      /*!
      *  \fn bool isDoingRightMove()
      *  \brief Returns true if bot decides to forward to right
      *  \return true if bot decides to turn to right
      */
      //bool isDoingRightMove() const { return m_bIsDoingRightMove;}
      bool isDoingRightMove() const { return m_iXMoveState == MOVE_RIGHT; }

      /*!
      *  \fn bool isDoingJump()
      *  \brief Returns true if bot decides to jumping
      *  \return true if bot decides to jumping
      */
      bool isDoingJump() { return m_bIsDoingJump;}

      /*!
      *  \fn void doingLeftMove_On()
      *  \brief Turns ON left move
      *
      *  if this funtion is called then bot forwards to left
      */
      //void doingLeftMove_On() { m_bIsDoingLeftMove = true;}
      void doingLeftMove_On() { m_iXMoveState = MOVE_LEFT;}

      /*!
      *  \fn void doingLeftMove_Off()
      *  \brief Turns OFF left move
      *
      *  if this funtion is called then bot stops to forward to left
      */
      //void doingLeftMove_Off() { m_bIsDoingLeftMove = false;}
      void doingLeftMove_Off() { m_iXMoveState = NO_MOVE;}

      /*!
      *  \fn void doingRightMove_On()
      *  \brief Turns ON right move
      *
      *  if this funtion is called then bot forwards to right
      */
      //void doingRightMove_On() { m_bIsDoingRightMove = true; }
      void doingRightMove_On() { m_iXMoveState = MOVE_RIGHT;}
      /*!
      *  \fn void doingRightMove_Off()
      *  \brief Turns OFF right move
      *
      *  if this funtion is called then bot stops to forward to right
      */
      //void doingRightMove_Off() { m_bIsDoingRightMove = false; }
      void doingRightMove_Off() { m_iXMoveState = NO_MOVE; }


      void moveOff()
      {
         // alter old positions to avoid false blocking detection
//         setOldPos(-1.0f, -1.0f);
//         setOldPos2(-2.0f, -2.0f);
         m_iOldPosEquaCounter = 0;
         m_iXMoveState = NO_MOVE;
      }

      bool isMoveOff() { return m_iXMoveState == NO_MOVE; }

      /*!
      *  \fn void doingJump_On()
      *  \brief Turns ON jumping
      *
      *  bot jumps after called it
      */
      void doingJump_On() { m_bIsDoingJump = true;}

      /*!
      *  \fn void doingJump_Off()
      *  \brief Turns OFF jumping
      *
      *  bot stops jumping after called it
      */
      void doingJump_Off() { m_bIsDoingJump = false;}


      bool isWalkingLeft() const { return m_bIsWalkingLeft; }

      void walkLeft() { m_bIsWalkingLeft = true; }
      void walkRight() { m_bIsWalkingLeft = false; }

      /*!
      *  \fn void update()
      *  \brief Updates thinking, weapon and moving of bot
      */
      void updateSpecial();

      GoalThink* const getBrain();
      void updateBlockingState();
      int BlockingState();
      bool isTouchingWaypoint (Waypoint& rWpt);
      void putInRightDirection (Waypoint& rWpt);

      /**
       * \fn bool isInRightDirection()
       * \brief Checks if bot is in right direction to reach next waypoint
       * \return true if bot is in right direction
       *
       * bot is in right direction if waypoint is at his left side and if he goes
       * to left, or if waypoint is at his right side and if he goes
       * to right
       */
      bool isInRightDirection(Waypoint& rWpt) const;


      const TargetingSystem* const  TargetSys() const;
      Character* TargetBot() const;
      SensoryMemory* const SensoryMem() const;

      void updateMovement();
      void setLastWaypointToReach(int idx);
      void setCurrentPathEdge(NavGraphEdge* pEdge);
      bool isBlocked() const;
      void setNoBlocked();
      void oneTryToUnlock();
      bool triedToUnlock() const;
      void resetTryToUnlock();
      bool canReachWaypoint(Waypoint& rWpt);
      void walkInOppositeWay();
};

#endif // BOT_H
