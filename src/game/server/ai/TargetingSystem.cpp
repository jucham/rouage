#include "TargetingSystem.h"

#include <game/server/Character.h>
#include <game/server/Bot.h>
#include "SensoryMemory.h"

TargetingSystem::TargetingSystem(Bot* pOwner) :
   m_pOwner(pOwner),
   m_pCurrentTarget(0)
{}

// Takes closest character (in all chars sensed recently) as target
void TargetingSystem::Update()
{
  float ClosestDistSoFar = MaxFloat;
  m_pCurrentTarget       = 0;

  //grab a list of all the opponents the owner can sense
  std::list<Character*> SensedBots;
  SensedBots = m_pOwner->SensoryMem()->GetListOfRecentlySensedOpponents();

  std::list<Character*>::const_iterator curBot = SensedBots.begin();
  for (curBot; curBot != SensedBots.end(); ++curBot)
  {
    //make sure the bot is alive and that it is not the owner
    if ((*curBot)->isAlive() && (*curBot != m_pOwner) )
    {
      //FIXME dists should be from center of characters
      float dist = Vec2DDistanceSq((*curBot)->Position(), m_pOwner->Position());

      if (dist < ClosestDistSoFar)
      {
        ClosestDistSoFar = dist;
        m_pCurrentTarget = *curBot;
      }
    }
  }
}

bool TargetingSystem::isTargetWithinFOV() const
{
  return m_pOwner->SensoryMem()->isOpponentWithinFOV(m_pCurrentTarget);
}

bool TargetingSystem::isTargetShootable() const
{
  return m_pOwner->SensoryMem()->isOpponentShootable(m_pCurrentTarget);
}

Vector2D TargetingSystem::GetLastRecordedPosition() const
{
  return m_pOwner->SensoryMem()->GetLastRecordedPositionOfOpponent(m_pCurrentTarget);
}

int TargetingSystem::GetTimeTargetHasBeenVisible() const
{
  return m_pOwner->SensoryMem()->GetTimeOpponentHasBeenVisible(m_pCurrentTarget);
}

int TargetingSystem::GetTimeTargetHasBeenOutOfView() const
{
  return m_pOwner->SensoryMem()->GetTimeOpponentHasBeenOutOfView(m_pCurrentTarget);
}
