#ifndef PLAYERWEAPONSYSTEM_H
#define PLAYERWEAPONSYSTEM_H

#include "WeaponSystem.h"

class GameServer;
class Character;
class Player;


///////////////////////////////////////////////////////////////////////////////
///
/// \class PlayerWeaponSystem PlayerWeaponSystem.h
/// \brief Weapon system for a player character
///
///////////////////////////////////////////////////////////////////////////////


class PlayerWeaponSystem : public WeaponSystem
{
   public:

      /// \brief Constructor
      ///
      /// \param pGame pointer to game
      /// \param pOwner player character that owns this weapon system
      ///
      ///
      PlayerWeaponSystem(GameServer* pGame, Player* pOwner);

      /// \fn void updateFacing()
      /// \brief Updates where weapon facing according to mousee cursor
      /// position
      ///
      void UpdateDirection();


      /// \fn bool isShooting()
      /// \brief Indicates if player wants to shoot with his weapon
      ///
      /// \return true if player wants to shoot with his weapon
      ///
      /// Player wants to shoot when he's pressing key or button that does
      /// this action
      ///
      bool IsShooting();

      bool ChangeWeapon();

      void Update();

      Character* Shooter() const;

   protected:
      Player* m_pOwner;

};

#endif // PLAYERWEAPONSYSTEM_H
