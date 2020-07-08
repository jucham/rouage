#ifndef ABSDIM_H_
#define ABSDIM_H_

// TODO remove bad coupling with game server
#include <game/server/EntityEnumerations.h>
#include <game/server/Tile.h>
#include <game/server/Character.h>
#include <game/server/Weapon.h>
#include <game/server/Rocket.h>
#include <game/server/Bullet.h>
#include <game/server/Trigger.h>

//
// Here are the dimensions of all world objects in meter unit.
//
class AbsDim
{
   public:
      static const float TILE_SIZE;
      static const float CHAR_W;
      static const float CHAR_H;
      static const float CHAR_ARM_W;
      static const float CHAR_ARM_H;
      static const float JETPACK_W;
      static const float JETPACK_H;
      static const Dimension WPDIMS[NUM_WEAPONS];
      static const float ROCKET_W;
      static const float ROCKET_H;
      static const float BULLET_W;
      static const float BULLET_H;
      static const float TRIGGER_W;
      static const float TRIGGER_H;

      static void initDimensions()
      {
         Tile::m_fWIDTH = Tile::m_fHEIGHT = TILE_SIZE;
         Character::m_fWIDTH = CHAR_W;
         Character::m_fHEIGHT = CHAR_H;
         Character::m_fARM_WIDTH = CHAR_ARM_W;
         Character::m_fARM_HEIGHT = CHAR_ARM_H;
         for (int i=0; i<NUM_WEAPONS; i++)
        	 Weapon::m_fWeaponDims[i] = AbsDim::WPDIMS[i];
         Rocket::m_fWIDTH = ROCKET_W;
         Rocket::m_fHEIGHT = ROCKET_H;
         Bullet::m_fWIDTH = BULLET_W;
         Bullet::m_fHEIGHT = BULLET_H;
         Trigger::m_fWIDTH = TRIGGER_W;
         Trigger::m_fHEIGHT = TRIGGER_H;
      }
};


#endif /* ABSDIM_H_ */
