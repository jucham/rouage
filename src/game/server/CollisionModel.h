#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include <vector>
#include <list>
#include "EntityEnumerations.h"
#include "Map.h"

class CollisionManager;
class Tile;
class Character;
class MovingEntity;

///////////////////////////////////////////////////////////////////////////////
///
/// \class CollisionModel CollisionModel.h
/// \brief Collision model is a base class for specialized collision models
///
/// Collision model is an abstract class which a base for specialized collision
/// models, by example CharacterCollisionModel which is specialized for
/// characters
///
///////////////////////////////////////////////////////////////////////////////
class CollisionModel
{
   public:

      /// \brief Constructor
      ///
      /// \param rTiles reference to vector of vector of map tiles
      ///
      CollisionModel(Map::TilePtr2dVector& rTiles) :
         m_rTiles(rTiles),
         m_iEntityType(ENTITY_NONE) {}

      /// \brief Destructor
      ///
      virtual ~CollisionModel() {}


      /// \fn virtual void collideWithEntity()=0
      /// \brief handles collision between one entity type and others
      ///
      /// if entity type is character this function iterates
      /// all characters and for each checks collisions with all others entities
      /// (others entities which can be collided with a character)
      ///
      virtual void CollideEntities()=0;

   protected:

      Map::TilePtr2dVector& m_rTiles; ///< reference to map tiles
      EntityType m_iEntityType;              ///< type of entity (character, trigger, etc.)
};

#endif // COLLISIONMODEL_H
