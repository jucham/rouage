#ifndef CHARACTERCOLLISIONMODELS_H
#define CHARACTERCOLLISIONMODELS_H

#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>
#include "CollisionModel.h"
#include "EntityEnumerations.h"
#include "Character.h"
#include "Tile.h"
#include "Projectile.h"
#include <base/AABBox2D.h>
#include "Map.h"
#include <base/debug.h>


class CharacterCollisionModel : public CollisionModel
{
protected:
	std::vector<Character*>& m_aChars;

public:

	CharacterCollisionModel(std::vector<Character*>& rChars, Map::TilePtr2dVector& rTiles) :

		CollisionModel(rTiles),
		m_aChars(rChars)
{}

	virtual ~CharacterCollisionModel() {}

	virtual bool checkAndDoCollision(Character* pChar, Tile* pTile) = 0;
	virtual void postCollisionUpdates(Character* pChar, bool entitiesCollided) = 0;

	void CollideEntities()
	{
		int   col = 0;
		int   lin = 0;
		bool  entitiesCollided = false;

		for(size_t n=0; n < m_aChars.size(); ++n )
		{

			Character* pCurChar = m_aChars[n];

			// dead or spawning characters mustn't be collided
			if ( pCurChar->isDead() || pCurChar->isSpawning() ) continue;

			assert ( pCurChar != 0 );

			col = Tile::tileContainsCenterColumn( pCurChar->BBox()->Center().x );
			lin = Tile::tileContainsCenterLine( pCurChar->BBox()->Center().y );

			entitiesCollided = false;

			// number of tiles in width and height
			int tilesInWidth = m_rTiles.size();
			int tilesInHeight = m_rTiles[0].size();

			// parse surrounding tiles
			for(int i=-1; i<=1; ++i)
			{
				for(int j=-2; j<=2; ++j)
				{
					// check for invalid vector indexes
					if ( col+i < 0 || lin+j < 0 || col+i > tilesInWidth-1 || lin+j > tilesInHeight-1 )
					{
						continue;
					}
					else
					{
						Tile* pt = m_rTiles.at(col+i).at(lin+j);
						assert (pt != NULL);

						if ( m_rTiles[col+i][lin+j]->hasEntityOnto(m_iEntityType) )
						{
							if (!entitiesCollided) {
								entitiesCollided = checkAndDoCollision( pCurChar, m_rTiles[col+i][lin+j] );
							}
							else
							{
								checkAndDoCollision( pCurChar, m_rTiles[col+i][lin+j] );
							}
						}
					}
				}
			}

			postCollisionUpdates(pCurChar, entitiesCollided );
		}
	}
};


// Triggers have particular behavior, they don't move after collision,
// but disappears.
class CollisionModelCharacterTrigger : public CharacterCollisionModel
{
public:
	CollisionModelCharacterTrigger(std::vector<Character*>& rChars, Map::TilePtr2dVector& rTiles) :
		CharacterCollisionModel(rChars, rTiles)
{
		m_iEntityType = ENTITY_TRIGGER;
}

	~CollisionModelCharacterTrigger() {}

	bool checkAndDoCollision(Character* pChar, Tile* pTile)
	{
		if( areEntitiesColliding(pChar, pTile) )
		{
			computeCollision(pChar, pTile);
			return true;
		}
		return false;
	}

	bool areEntitiesColliding(Character* pChar, Tile* pTile)
	{
		Trigger* t = pTile->GetTriggerOnto();
		if( !t->isActive() )
		{
			return false;
		}

		return pChar->BBox()->isOverlappedWith( *(t->BBox()) );
	}

	void computeCollision(Character* pChar, Tile* pTile)
	{
		Trigger* t = pTile->GetTriggerOnto();
		t->setTouched();
		t->setCharTriggered(pChar);
	}

	void postCollisionUpdates(Character* pChar, bool entitiesCollided)
	{
		return;
	}

};


// Projectiles have a short life, whether they touch characters they die
class CollisionModelCharacterProjectile : public CharacterCollisionModel
{
public:
	CollisionModelCharacterProjectile(std::vector<Character*>& rChars, Map::TilePtr2dVector& rTiles) :
		CharacterCollisionModel(rChars, rTiles)
{
		m_iEntityType = ENTITY_PROJECTILE;
}

	~CollisionModelCharacterProjectile() {}


	bool checkAndDoCollision(Character* pChar, Tile* pTile)
	{
		std::list<Projectile*> projList( pTile->GetProjectilesOnto() );

		std::list<Projectile*>::iterator pCurProj = projList.begin();

		while (pCurProj != projList.end())
		{
			if ( areEntitiesColliding(pChar, pTile, *pCurProj) )
			{
				computeCollision(pChar, *pCurProj );
			}

			++pCurProj;
		}

		return false;
	}

	bool areEntitiesColliding(Character* pChar, Tile* pTile, Projectile* pProj)
	{
		return  pChar->BBox()->isOverlappedWith( *(pProj->BBox()) );
	}

	void computeCollision(Character* pChar, Projectile* pProj)
	{
		if (pChar != pProj->GetOwner() || pProj->wasShotFrom(500) )
		{
			pProj->setDead();
			pProj->removeTileSpot();
			pChar->takeDamage( pProj );

		}
	}

	void postCollisionUpdates(Character* pChar, bool entitiesCollided)
	{
		return;
	}

};


class CollisionModelCharacterTile : public CharacterCollisionModel
{
public:
	CollisionModelCharacterTile(std::vector<Character*>& rChars, Map::TilePtr2dVector& rTiles) :
		CharacterCollisionModel(rChars, rTiles)
{

		m_iEntityType = ENTITY_TILE;
}

	~CollisionModelCharacterTile() {}

	bool checkAndDoCollision(Character* pChar, Tile* pTile)
	{
		assert (pChar!=0);
		assert (pTile!=0);

		bool entitiesCollided = false;

		if( areEntitiesColliding(pChar, pTile) )
		{
			entitiesCollided = true;
			computeCollision(pChar, pTile);
		}

		return entitiesCollided;
	}

	bool areEntitiesColliding(Character* pChar, Tile* pTile)
	{
		assert (pChar!=0);
		assert (pTile!=0);

		return
				pTile->isSolid()
				&&
				pChar->BBox()->isOverlappedWith( AABBox2D(pTile->Position(), Tile::m_fWIDTH, Tile::m_fHEIGHT) );
	}

	void computeCollision(Character* pChar, Tile* pTile)
	{
		assert (pChar!=0);
		assert (pTile!=0);

		float xOverlapOld = 0.0f;
		float yOverlapOld = 0.0f;
		bool     isHorizontalCollision = true;
		AABBox2D oldBBox(pChar->OldPosition(),  Character::m_fWIDTH, Character::m_fHEIGHT);

		oldBBox.isOverlappedWith( AABBox2D(pTile->Position(),
				Tile::m_fWIDTH,
				Tile::m_fHEIGHT),
				&xOverlapOld,
				&yOverlapOld);

		if (xOverlapOld >= yOverlapOld)
		{
			isHorizontalCollision = false;
		}

		if (!pTile->isSolidSide(Tile::left) && !pTile->isSolidSide(Tile::right))
		{
			isHorizontalCollision = false;
		}

		// fait la collision
		if (isHorizontalCollision)
		{
			if (pChar->Velocity().x > 0 && pTile->isSolidSide(Tile::left))
			{
				pChar->BBox()->setPositionX( pTile->Position().x - pChar->BBox()->Width() - 0.01f );
			}
			else if (pChar->Velocity().x < 0 && pTile->isSolidSide(Tile::right))
			{
				pChar->BBox()->setPositionX( pTile->Position().x+Tile::m_fWIDTH + 0.01f );
			}

			pChar->hitSide();
			pChar->setPosition(pChar->BBox()->Position());
		}
		else
		{
			if(pChar->Velocity().y >= 0 && pTile->isSolidSide(Tile::bottom)) //hit ceiling
			{
				pChar->BBox()->setPositionY( pTile->Position().y-Character::m_fHEIGHT - 0.01f ); //alignment with tile
				pChar->setPosition(pChar->BBox()->Position());
				pChar->hitCeiling();
			}
			else if (pTile->isSolidSide(Tile::top)) //hit ground
			{
				pChar->BBox()->setPositionY( pTile->Position().y+Tile::m_fHEIGHT ); //alignment with tile
				pChar->setPosition(pChar->BBox()->Position());
				pChar->setToGround();
			}
		}
	}

	void postCollisionUpdates(Character* pChar, bool entitiesCollided)
	{
		assert (pChar!=0);

		// character don't collide tile so if it isn't to ground then it falls.
		if(!entitiesCollided && !pChar->isFalling() && !pChar->isJumping() && !pChar->IsJetpacking())
			pChar->fall();
	}

};

#endif // CHARACTERCOLLISIONMODELS_H
