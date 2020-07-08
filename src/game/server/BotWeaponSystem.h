#ifndef BOTWEAPONSYSTEM_H
#define BOTWEAPONSYSTEM_H

#include "WeaponSystem.h"

/*!
*  \class BotWeaponSystem
*  \brief Weapon system specialized for a bot
*  \author Julien Champalbert
*  \version 0.1
*/

class Bot;

class BotWeaponSystem : public WeaponSystem
{
   public:

      /*!
      *  \brief Constructor
      *  \param pGame pointer to game
      *  \param pOwner pointer to character possess this weapon system
      */
      BotWeaponSystem(GameServer* pGame,
                        Bot* pOwner,
                        int reactionTime,
                        float aimAccuracy,
                        int aimPersistance);

      /*!
      *  \brief Destructor
      */
      ~BotWeaponSystem();

      /*!
      *  \fn void selectWeapon()
      *  \brief Selects weapon which is best for a done situation
      */
      bool changeWeapon();

      /*!
      *  \fn updateFacing()
      *  \brief Updates direction of weapon
      */
      void updateFacing();

      /*!
      *  \fn bool isShooting()
      *  \brief Returns true if bot is shooting
      *  \return true if bot is shooting
      */
      bool isShooting();

      void update();

      //predicts where the target will be by the time it takes the current weapon's
      //projectile type to reach it. Used by TakeAimAndShoot
      //Vector2D    PredictFuturePositionOfTarget() const;

      //adds a random deviation to the firing angle not greater than m_dAimAccuracy
      //rads
      //void        AddNoiseToAim(Vector2D& AimingPos) const;

      bool RotateFacingTowardPosition( const Vector2D& target, bool fullRotation);

      Character* Shooter() const;

   protected:

      Bot*  m_pOwner;

      //this is the minimum amount of time a bot needs to see an opponent before
      //it can react to it. This variable is used to prevent a bot shooting at
      //an opponent the instant it becomes visible.
      int m_iReactionTime;

      //each time the current weapon is fired a certain amount of random noise is
      //added to the the angle of the shot. This prevents the bots from hitting
      //their opponents 100% of the time. The lower this value the more accurate
      //a bot's aim will be. Recommended values are between 0 and 0.2 (the value
      //represents the max deviation in radians that can be added to each shot).
      float m_fAimAccuracy;

      //the amount of time a bot will continue aiming at the position of the target
      //even if the target disappears from view.
      int m_iAimPersistance;

      float m_fMaxTurnRate;



};

#endif // BOTWEAPONSYSTEM_H
