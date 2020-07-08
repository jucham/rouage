#ifndef GOALDODGE_H
#define GOALDODGE_H

#include "Goal.h"
#include <game/server/Bot.h>
#include <game/server/GameServer.h>

///
/// \class GoalDodge GoalDodge.h
/// \brief Implements a dodge behavior on attack
///
class GoalDodge : public Goal<Bot>
{
   public:

      GoalDodge(Bot* pBot);

      void Activate();
      int  Process();
      void Terminate();

      /// \brief indicates if bot must change dodge type
      bool mustChangeDodge() const;

      /// \brief select randomly a new dodge type with random parameters
      void resetDodgeParams();

      /// \brief if time between each dodge action is elapsed that returns true
      bool canRetryDodge() const;

   protected:

      enum
      {
         DODGE_JUMP,
         DODGE_WALK,
         DODGE_DEADPAN
      }; ///< enumeration of differents attack types

      int m_iAttackType;               ///< type of attack
      int m_iMaxDodgeNum;              ///< max number of dodge done for current attack type
      int m_iCurDodgeNum;              ///< current number of dodges done for current attack type
      int m_iDelayBetweenDodgeRestart; ///< delay between each dodge actions
      int m_iTimeDodgeStarted;         ///< time when dodge started

};




#endif // GOALDODGE_H
