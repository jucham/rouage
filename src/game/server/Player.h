#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Character.h"

class GameServer;


///////////////////////////////////////////////////////////////////////////////
///
/// \class Player Player.h
/// \brief This class implements a player character
///
/// A player is a character controlled by a user by means of mouse and keyboard
///
///////////////////////////////////////////////////////////////////////////////
class Player : public Character
{
   public:

      /// \brief Constructor
      /// \param pGame pointer to game
      /// \param name name of player character
      ///
      Player(GameServer* pGame, int clientId);

      /// \fn bool isDoingLeftMove()
      /// \brief indicates if player wants to move to left
      ///
      /// \return true if player wants to move to left
      ///
      /// For a player, want to move to left means press the key which
      /// do this action
      ///
      bool isDoingLeftMove() const;

      /// \fn bool isDoingRightMove()
      /// \brief indicates if player wants to move to right
      ///
      /// \return true if player wants to move to right
      ///
      /// For a player, want to move to right means press the key which
      /// do this action
      ///
      bool isDoingRightMove() const;

      /// \fn bool isDoingJump()
      /// \brief indicates if player wants to jumping
      ///
      /// \return true if player wants to jumping
      ///
      /// For a player, want to jumping means press the key which
      /// do this action
      ///
      bool isDoingJump();

      bool IsJetpacking() const;

      /// \fn void update()
      /// \brief Updates weapon and moving of player
      ///
      void updateSpecial();

};


#endif // PLAYER_H_INCLUDED
