#include "GoalHuntTarget.h"

#include "GoalTypes.h"
#include <game/server/Bot.h>

GoalHuntTarget::GoalHuntTarget(Bot* pOwner) :
   GoalComposite<Bot>(pOwner, GOAL_HUNT_TARGET)
{}

void GoalHuntTarget::Activate()
{

}

int  GoalHuntTarget::Process() {}

void GoalHuntTarget::Terminate() {}
