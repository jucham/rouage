#include "GoalDodge.h"

#include "TargetingSystem.h"


GoalDodge::GoalDodge(Bot* pBot) : Goal<Bot>(pBot, GOAL_DODGE),
      m_iAttackType(0),
      m_iMaxDodgeNum(0),
      m_iCurDodgeNum(1),
      m_iDelayBetweenDodgeRestart(0),
      m_iTimeDodgeStarted(0)
{}


void GoalDodge::Activate()
{
   m_iStatus = active;
   resetDodgeParams();
}

int  GoalDodge::Process()
{
   ActivateIfInactive();

   //if target goes out of view terminate
   if (!m_pOwner->TargetSys()->isTargetWithinFOV()) { m_iStatus = completed; }

   // we must change dodge type so reset dodge params
   if ( mustChangeDodge() ) { resetDodgeParams(); }

   // do again the current dodge if time between each is elapsed
   if ( canRetryDodge() )
   {
      m_iTimeDodgeStarted = Time::ms();

      switch(m_iAttackType)
      {
         case DODGE_JUMP:
            //TODO: test if character is near
            m_pOwner->moveOff();
            m_pOwner->doingJump_On();
         break;

         case DODGE_WALK:
            m_pOwner->walkInOppositeWay();
         break;

         case DODGE_DEADPAN:
            m_pOwner->moveOff();
         break;
      }

      m_iCurDodgeNum++;
   }

   return m_iStatus;
}

void GoalDodge::Terminate()
{
   m_pOwner->moveOff();
}

bool GoalDodge::mustChangeDodge() const
{
   return m_iCurDodgeNum > m_iMaxDodgeNum;
}

void GoalDodge::resetDodgeParams()
{
      //const char dodgeWalkLimit = 100;
      const char dodgeJumpLimit = 50;
      const char dodgeDeadpanLimit = 0;

      int attackValue = RandInt(0,100);
      m_iMaxDodgeNum = RandInt(1,2);
      m_iCurDodgeNum = 0;

//      if ( attackValue < dodgeDeadpanLimit )
//      {
//          m_iAttackType = DODGE_DEADPAN;
//          m_iDelayBetweenDodgeRestart = RandInt(500, 1000);
//      }
//      else if ( attackValue < dodgeJumpLimit)
//      {
//         m_iAttackType = DODGE_JUMP;
//         m_iDelayBetweenDodgeRestart = RandInt(1500, 2000);
//      }
//      else
//      {
         m_iAttackType = DODGE_WALK;
         m_iDelayBetweenDodgeRestart = RandInt(1000, 1500);
      //}

}

bool GoalDodge::canRetryDodge() const
{
   int curTime = Time::ms();
   return  curTime - m_iTimeDodgeStarted > m_iDelayBetweenDodgeRestart;
}

