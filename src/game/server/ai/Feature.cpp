#include "Feature.h"
#include "PathPlanner.h"
#include <game/server/Bot.h>
#include <game/server/GameServer.h>
#include <game/server/Map.h>
#include <game/server/Weapon.h>
#include <game/server/WeaponSystem.h>


float Feature::Health(Bot* pBot)
{
  assert ( pBot != 0 );

  return (float)pBot->GetLife() / (float)pBot->GetMaxLife();

}

float Feature::DistanceToItem(Bot* pBot, int ItemType, int type)
{
  assert ( pBot != 0 );

  //determine the distance to the closest instance of the item type
  float DistanceToItem = pBot->GetGameServer()->GetMap()->GetPathPlanner().getCostToClosestItem(pBot->Position(), ItemType, type);

  //if the previous method returns a negative value then there is no item of
  //the specified type present in the game world at this time.
  if (DistanceToItem < 0 ) return 1;

  //these values represent cutoffs. Any distance over MaxDistance results in
  //a value of 0, and value below MinDistance results in a value of 1
  const float MaxDistance = 1000.0f;
  const float MinDistance = 100.0f;

  Clamp(DistanceToItem, MinDistance, MaxDistance);

  return DistanceToItem / MaxDistance;
}


float Feature::IndividualWeaponStrength(Bot* pBot, int WeaponType)
{
  //grab a pointer to the gun (if the bot owns an instance)
  Weapon* wp = pBot->WeaponSys()->getWeaponFromInventory(WeaponType);

  if (wp != 0)
  {
      return wp->AmmoCount() / wp->MaxAmmoCount();
  }
  else
  {
      return 0.0f;
  }
}


float Feature::TotalWeaponStrength(Bot* pBot)
{
  const float MaxNumBullets = pBot->WeaponSys()->getWeaponFromInventory(WP_PISTOL)->MaxAmmoCount();
  const float MaxNumRockets = pBot->WeaponSys()->getWeaponFromInventory(WP_ROCKET_LAUNCHER)->MaxAmmoCount();
  const float TotalAmmo = MaxNumBullets + MaxNumRockets;

  float NumBullets = pBot->WeaponSys()->getWeaponFromInventory(WP_PISTOL)->AmmoCount();
  float NumRockets = pBot->WeaponSys()->getWeaponFromInventory(WP_ROCKET_LAUNCHER)->AmmoCount();

  const float Tweaker = 0.1f;

  return Tweaker + (1-Tweaker)*(NumBullets + NumRockets) / TotalAmmo;
}

