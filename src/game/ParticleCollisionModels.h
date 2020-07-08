#ifndef PARTICLECOLLISIONMODEL_H_INCLUDED
#define PARTICLECOLLISIONMODEL_H_INCLUDED

#include <game/server/Tile.h>

class ParticleCollisionModel : public CollisionModel
{
protected:
	std::list<Particle*>& m_rParts;

public:

	ParticleCollisionModel(std::list<Particle*>& rParts, Map::TilePtr2dVector& rTiles) :

		CollisionModel(rTiles),
		m_rParts(rParts)
{}

	virtual ~ParticleCollisionModel() {}

	virtual bool checkAndDoCollision(Particle* pPart, Tile* pTile) = 0;

	
	void CollideEntities()
	{
		int   col = 0;
		int   lin = 0;
		bool  entitiesCollided = false;

		// number of tiles in width and height
		int tilesInWidth = static_cast<int> (m_rTiles.size());
		int tilesInHeight = static_cast<int> (m_rTiles[0].size());

		std::list<Particle*>::iterator pCurPart;
		for ( pCurPart = m_rParts.begin(); pCurPart != m_rParts.end(); ++pCurPart)
		{
			Particle* pPart = *pCurPart;

			if( ! pPart->mustBeCollided() ) { continue; }

			col = Tile::tileContainsCenterColumn( pPart->BBox()->Center().x );
			lin = Tile::tileContainsCenterLine( pPart->BBox()->Center().y );
			entitiesCollided = false;

			for(int i=-1; i<=1; ++i)
			{
				for(int j=-1; j<=1; ++j)
				{
					// check for invalid vector indexes
					if ( col+i < 0 || lin+j < 0 || col+i > tilesInWidth-1 || lin+j > tilesInHeight-1 )
					{
						continue;
					}

					if (!entitiesCollided)
					{
						entitiesCollided = checkAndDoCollision( pPart, m_rTiles[col+i][lin+j] );
					}
					else
					{
						checkAndDoCollision( pPart, m_rTiles[col+i][lin+j] );
					}
				}
			}
		}
	}
};

class CollisionModelParticleTile : public ParticleCollisionModel
{
public:
	CollisionModelParticleTile( std::list<Particle*>& rParts, Map::TilePtr2dVector& rTiles) :
		ParticleCollisionModel( rParts, rTiles )
{
		m_iEntityType = ENTITY_TILE;
}

	~CollisionModelParticleTile() {}


	bool checkAndDoCollision(Particle* pPart, Tile* pTile)
	{
		if ( areEntitiesColliding( pPart, pTile ) )
		{
			computeCollision( pPart, pTile );
		}

		return false;
	}

	bool areEntitiesColliding(Particle* pPart, Tile* pTile)
	{
		return
				pTile->isSolid()
				&&
				pPart->BBox()->isOverlappedWith( AABBox2D(pTile->Position(), Tile::m_fWIDTH, Tile::m_fHEIGHT) );
	}

	void computeCollision(Particle* pPart, Tile* pTile)
	{
		float xOverlapOld = 0.0;
		float yOverlapOld = 0.0;
		bool  isHorizontalCollision = true;
		AABBox2D oldBBox(pPart->OldPosition(),  pPart->BBox()->Width(), pPart->BBox()->Height());

		oldBBox.isOverlappedWith( AABBox2D(pTile->Position(),
				Tile::m_fWIDTH,
				Tile::m_fHEIGHT),
				&xOverlapOld,
				&yOverlapOld);

		if (xOverlapOld >= yOverlapOld)
		{
			isHorizontalCollision = false;
		}

		if (isHorizontalCollision)
		{
			// RIGHT COL
			if ((pPart->Velocity().x > 0)
					&& pTile->isSolidSide(Tile::left))
			{
				pPart->BBox()->setPositionX( pTile->Position().x - pPart->BBox()->Width() );
				pPart->setPosition(pPart->BBox()->Position());
				pPart->onCollide(m_iEntityType, HORIZ);

			}

			// LEFT COL
			else if ((pPart->Velocity().x <= 0)
					&& pTile->isSolidSide(Tile::right))
			{
				pPart->BBox()->setPositionX( pTile->Position().x+Tile::m_fWIDTH );
				pPart->setPosition(pPart->BBox()->Position());
				pPart->onCollide(m_iEntityType, HORIZ);

			}
		}
		else
		{
			//hit ceiling
			if(pPart->Velocity().y > 0
					&& pTile->isSolidSide(Tile::bottom))
			{
				pPart->BBox()->setPositionY( pTile->Position().y - pPart->BBox()->Height() ); //alignment with tile
				pPart->setPosition(pPart->BBox()->Position());
				pPart->onCollide(m_iEntityType, VERT);
			}

			//hit ground
			else if (pTile->isSolidSide(Tile::top))
			{
				pPart->BBox()->setPositionY( pTile->Position().y+Tile::m_fHEIGHT ); //alignment with tile
				pPart->setPosition(pPart->BBox()->Position());
				pPart->onCollide(m_iEntityType, VERT);
			}
		}
	}

};

#endif // PARTICLECOLLISIONMODEL_H_INCLUDED
