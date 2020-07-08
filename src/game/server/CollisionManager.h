#ifndef COLLISIONMANAGER_H_INCLUDED
#define COLLISIONMANAGER_H_INCLUDED

class GameServer;
class CollisionModel;

///////////////////////////////////////////////////////////////////////////////
///
/// \class CollisionManager CollisionManager.h
/// \brief Collision manager handles all types of collision
///
/// Collision manager contains functions process all existent collision types,
/// except collision with waypoints.
///
///////////////////////////////////////////////////////////////////////////////
class CollisionManager
{
   public:

      /// \brief Constructor
      ///
      /// \param pGame pointer to game
      ///
      CollisionManager(GameServer* pGame);
      ~CollisionManager();

      /// \fn void collideCharacters()
      /// \brief Computes all collision between characters and other entities
      ///
      void collideCharacters();

      /// \fn void collideProjectiles()
      /// \brief Computes all collision between projectiles and other entities
      ///
      void collideProjectiles();

   private:

      GameServer* m_pGameServer;

      CollisionModel* m_pColModCharTrig;
      CollisionModel* m_pColModCharProj;
      CollisionModel* m_pColModCharTile;
      CollisionModel* m_pColModProjTile;

};

#endif // COLLISIONMANAGER_H_INCLUDED
