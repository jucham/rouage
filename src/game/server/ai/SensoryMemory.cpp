#include "SensoryMemory.h"

#include <game/server/Character.h>
#include <game/server/Bot.h>
#include <game/server/GameServer.h>
#include <game/server/WeaponSystem.h>
#include <base/RayTracing.h>
#include <base/debug.h>

SensoryMemory:: SensoryMemory(Bot* pOwner, int MemorySpan) :
   m_pOwner(pOwner),
   m_iMemorySpan(MemorySpan)
{}

void SensoryMemory::MakeNewRecordIfNotAlreadyPresent(Character* pOpponent)
{
  //else check to see if this Opponent already exists in the memory. If it doesn't,
  //create a new record
  if (m_MemoryMap.find(pOpponent) == m_MemoryMap.end())
  {
    m_MemoryMap[pOpponent] = MemoryRecord();
  }
}

void SensoryMemory::RemoveBotFromMemory(Character* pOpponent)
{
  MemoryMap::iterator record = m_MemoryMap.find(pOpponent);

  if (record != m_MemoryMap.end())
  {
    m_MemoryMap.erase(record);
  }
}

void SensoryMemory::UpdateWithSoundSource(Character* pNoiseMaker)
{
  //make sure the bot being examined is not this bot
  if (m_pOwner != pNoiseMaker)
  {
    //if the bot is already part of the memory then update its data, else
    //create a new memory record and add it to the memory
    MakeNewRecordIfNotAlreadyPresent(pNoiseMaker);

    MemoryRecord& info = m_MemoryMap[pNoiseMaker];

    //test if there is LOS between bots
    if (0)//m_pOwner->GetGame()->isLOSOkay(m_pOwner->Position(), pNoiseMaker->Position()))
    {
      info.bShootable = true;

     //record the position of the bot
      info.vLastSensedPosition = pNoiseMaker->Position();
    }
    else
    {
      info.bShootable = false;
    }

    //record the time it was sensed
    info.iTimeLastSensed = Time::ms();
  }
}

void SensoryMemory::UpdateVision()
{
   //for each bot in the world test to see if it is visible to the owner of
   //this class
   const std::vector<Character*>& opponents = m_pOwner->GetGameServer()->Characters();
   std::vector<Character*>::const_iterator curBot;
   for (curBot = opponents.begin(); curBot!=opponents.end(); ++curBot)
   {
      //make sure the bot being examined is not this bot
      if (m_pOwner != *curBot)
      {
         //make sure it is part of the memory map
         MakeNewRecordIfNotAlreadyPresent(*curBot);

         //get a reference to this bot's data
         MemoryRecord& info = m_MemoryMap[*curBot];

         // checks if current target character is into FOV
         AABBox2D fovBox
         (
//            Vector2D(m_pOwner->Position().x + Character::m_fWIDTH/2 - m_pOwner->GetGame()->RenderMgr()->FrameWidth()/2 ,
//                     m_pOwner->Position().y + Character::m_fHEIGHT/2 - m_pOwner->GetGame()->RenderMgr()->FrameHeight()/2 ),
//            m_pOwner->GetGame()->RenderMgr()->FrameWidth(),
//            m_pOwner->GetGame()->RenderMgr()->FrameHeight()
         );

    
         bool isIntoFOV = false;// (*curBot)->BBox()->isOverlappedWith( fovBox );

         if(isIntoFOV)
         {
            info.iTimeLastSensed     = Time::ms();
            info.vLastSensedPosition = (*curBot)->Position();
            info.iTimeLastVisible    = Time::ms();

            if (info.bWithinFOV == false)
            {
               info.iTimeBecameVisible = info.iTimeLastSensed;
            }

            info.bWithinFOV = true;

            Vector2D targetPos ( (*curBot)->Position() );
            targetPos.x += Character::m_fWIDTH/2;
            targetPos.y += Character::m_fHEIGHT/2;

            if (RayTracing::canTraceRay(m_pOwner->WeaponSys()->WeaponPos(),
                                        targetPos,
                                        m_pOwner->GetGameServer()->GetMap() ))
            {
               info.bShootable = true;
            }
            else
            {
               info.bShootable = false;
            }
         }
         else
         {
            info.bWithinFOV = false;
            info.bShootable = false;
         }
      }
   }
}

std::list<Character*> SensoryMemory::GetListOfRecentlySensedOpponents() const
{
  //this will store all the opponents the bot can remember
  std::list<Character*> opponents;

  float CurrentTime = Time::ms();

  MemoryMap::const_iterator curRecord = m_MemoryMap.begin();
  for (curRecord; curRecord!=m_MemoryMap.end(); ++curRecord)
  {
    //if this bot has been updated in the memory recently, add to list
    if ( (CurrentTime - curRecord->second.iTimeLastSensed) <= m_iMemorySpan)
    {
      opponents.push_back(curRecord->first);
    }
  }

  return opponents;
}

bool SensoryMemory::isOpponentShootable(Character* pOpponent) const
{
  MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

  if (it != m_MemoryMap.end())
  {
    return it->second.bShootable;
  }

  return false;
}

bool  SensoryMemory::isOpponentWithinFOV(Character* pOpponent) const
{
  MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

  if (it != m_MemoryMap.end())
  {
    return it->second.bWithinFOV;
  }

  return false;
}

Vector2D  SensoryMemory::GetLastRecordedPositionOfOpponent(Character* pOpponent) const
{
  MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

  if (it != m_MemoryMap.end())
  {
    return it->second.vLastSensedPosition;
  }

  assert ("< SensoryMemory::GetLastRecordedPositionOfOpponent>: Attempting to get position of unrecorded bot");
}


int  SensoryMemory::GetTimeOpponentHasBeenVisible(Character* pOpponent) const
{
  MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

  if (it != m_MemoryMap.end() && it->second.bWithinFOV)
  {
    return Time::ms() - it->second.iTimeBecameVisible;
  }

  return 0;
}

int SensoryMemory::GetTimeOpponentHasBeenOutOfView(Character* pOpponent) const
{
  MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

  if (it != m_MemoryMap.end())
  {
    return Time::ms() - it->second.iTimeLastVisible;
  }

  return MaxInt;
}

int  SensoryMemory::GetTimeSinceLastSensed(Character* pOpponent) const
{
  MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

  if (it != m_MemoryMap.end() && it->second.bWithinFOV)
  {
    return Time::ms() - it->second.iTimeLastSensed;
  }

  return 0;
}

void  SensoryMemory::RenderBoxesAroundRecentlySensed() const
{
  std::list<Character*> opponents = GetListOfRecentlySensedOpponents();
  std::list<Character*>::const_iterator it;
  for (it = opponents.begin(); it != opponents.end(); ++it)
  {
//    gdi->OrangePen();
//    Vector2D p = (*it)->Pos();
//    float   b = (*it)->BRadius();
//
//    gdi->Line(p.x-b, p.y-b, p.x+b, p.y-b);
//    gdi->Line(p.x+b, p.y-b, p.x+b, p.y+b);
//    gdi->Line(p.x+b, p.y+b, p.x-b, p.y+b);
//    gdi->Line(p.x-b, p.y+b, p.x-b, p.y-b);
  }

}
