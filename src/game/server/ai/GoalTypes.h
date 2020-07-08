#ifndef GOALTYPES_H
#define GOALTYPES_H

#include <string>

///
/// \file GoalTypes.h
/// \brief This header contains goal types
///

enum
{
   GOAL_THINK,
   GOAL_EXPLORE,
   GOAL_ATTACK_TARGET,
   GOAL_DODGE,
   GOAL_HUNT_TARGET,
   GOAL_FOLLOW_PATH,
   GOAL_WALK_ON_EDGE,
   GOAL_JUMP_ON_EDGE,
   GOAL_MOVE_TO_WAYPOINT,
   GOAL_GET_HEALTH,
   GOAL_GET_PISTOL,
   GOAL_GET_ROCKET_LAUNCHER
}; ///< enumerates all goal types


/// \brief returns name of a goal for a given type
static std::string nameOfType(const int type)
{
   switch(type)
   {
      case GOAL_THINK:
         return "Think";

      case GOAL_EXPLORE:
         return "Explore";

      case GOAL_ATTACK_TARGET:
         return "Attack Target";

      case GOAL_DODGE:
         return "Dodge";

      case GOAL_FOLLOW_PATH:
         return "Follow path";

      case GOAL_WALK_ON_EDGE:
         return "Walk on edge";

      case GOAL_JUMP_ON_EDGE:
         return "Jump on edge";

      case GOAL_MOVE_TO_WAYPOINT:
         return "Move to waypoint";

      case GOAL_GET_HEALTH:
         return "Get Health";

      case GOAL_GET_PISTOL:
         return "Get Pistol";

      case GOAL_GET_ROCKET_LAUNCHER:
         return "Get rocket launcher";

      default:
         return "Unknow goal type";
   }
}


#endif // GOALTYPES_H
