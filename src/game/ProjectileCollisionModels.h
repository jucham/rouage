#ifndef PROJECTILECOLLISIONMODELS_H
#define PROJECTILECOLLISIONMODELS_H

#include <list>
#include <game/server/CollisionModel.h>
#include <game/server/EntityEnumerations.h>
#include <game/server/Tile.h>
#include <game/server/Projectile.h>
#include <game/server/Map.h>
#include <base/AABBox2D.h>
#include <base/Ray.h>
#include <base/debug.h>


class ProjectileCollisionModel : public CollisionModel
{
protected:
	std::list<Projectile*>& m_rProjs;

public:

	ProjectileCollisionModel(std::list<Projectile*>& rProj, Map::TilePtr2dVector& rTiles) :

		CollisionModel(rTiles),
		m_rProjs(rProj)
{}

	virtual ~ProjectileCollisionModel() {}

	virtual bool checkAndDoCollision(Projectile* pProj, Tile* pTile) = 0;

	void CollideEntities()
	{

		int   col = 0;
		int   lin = 0;
		bool  entitiesCollided = false;

		// number of tiles in width and height
		int tilesInWidth = static_cast<int> (m_rTiles.size());
		int tilesInHeight = static_cast<int> (m_rTiles[0].size());

		std::list<Projectile*>::iterator pCurProj = m_rProjs.begin();
		while (pCurProj != m_rProjs.end())
		{
			col = Tile::tileContainsCenterColumn( (*pCurProj)->BBox()->Center().x );
			lin = Tile::tileContainsCenterLine( (*pCurProj)->BBox()->Center().y );
			entitiesCollided = false;

			// projectile out of map bounds
			if ( col < 0 || lin < 0 || col > tilesInWidth-1 || lin > tilesInHeight-1 ) {
				(*pCurProj)->setDead();
				++pCurProj;
				continue;
			}

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
						entitiesCollided = checkAndDoCollision( *pCurProj, m_rTiles[col+i][lin+j] );
					}
					else
					{
						break;
					}
				}
			}
			++pCurProj;
		}
	}
};

// Projectiles have a short life, whether they touch tiles, they
// explode suddenly.
class CollisionModelProjectileTile : public ProjectileCollisionModel
{
public:

	CollisionModelProjectileTile( std::list<Projectile*>& rProj, Map::TilePtr2dVector& rTiles) :
		ProjectileCollisionModel( rProj, rTiles )
{
		m_iEntityType = ENTITY_TILE;
}

	~CollisionModelProjectileTile() {}


	bool checkAndDoCollision(Projectile* pProj, Tile* pTile)
	{
		bool isCollided = areEntitiesColliding( pProj, pTile );

		if ( isCollided )
		{
			computeCollision( pProj, pTile );
		}

		return isCollided;
	}

	bool areEntitiesColliding(Projectile* pProj, Tile* pTile)
	{
		return
				pTile->isSolid()
				&&
				pProj->BBox()->isOverlappedWith( AABBox2D(pTile->Position(), Tile::m_fWIDTH, Tile::m_fHEIGHT) );
	}

	void computeCollision(Projectile* pProj, Tile* pTile)
	{
		//TODO do event handler

//#define CRAFTING
#if defined(CRAFTING)
		Vector2D tilePos = pTile->Position() + Vector2D(Tile::m_fWIDTH/2, Tile::m_fHEIGHT/2);
		int col = Tile::tileContainsCenterColumn( tilePos.x );
		int lin = Tile::tileContainsCenterLine( tilePos.y );
		int tilesInWidth = static_cast<int> (m_rTiles.size());
		int tilesInHeight = static_cast<int> (m_rTiles[0].size());
		bool boundTile = (col == 0 || lin == 0 || col == tilesInWidth-1 || lin == tilesInHeight-1);

		if (!boundTile) {

			// left tile
			if (m_rTiles[col-1][lin]->isSolid()) {
				m_rTiles[col-1][lin]->setSideSolidity(Tile::right, true);
			}

			//right tile
			if (m_rTiles[col+1][lin]->isSolid()) {
				m_rTiles[col+1][lin]->setSideSolidity(Tile::left, true);
			}

			// top tile
			if (m_rTiles[col][lin+1]->isSolid()) {
				m_rTiles[col][lin+1]->setSideSolidity(Tile::bottom, true);
			}

			// bottom tile
			if (m_rTiles[col][lin-1]->isSolid()) {
				m_rTiles[col][lin-1]->setSideSolidity(Tile::top, true);
			}

			m_rTiles[col][lin]->setSolidity(false);

			GameServer::Event event;
			event.type = EVENT_DESTROY_TILE;
			event.pos = tilePos;
			event.ownerClientId = pProj->GetOwner()->GetClientId();
			pProj->GetOwner()->GetGameServer()->AddEvent(event);

		}
#endif

		//FIXME cause crash because outbound trick
		//         Vector2D colOrigin;
		//
		//         OrientTBLR colOri = resolveCollision(pProj,
		//                                                   pTile,
		//                                                   &colOrigin);
		//
		//         switch ( colOri )
		//         {
		//            case TOP:
		//               pProj->setPosition( colOrigin );
		//               //pProj->onVerticalHit();
		//               pProj->onCollide(m_iEntityType, VERT);
		//               break;
		//
		//            case BOTTOM:
		//               colOrigin.y -= pProj->Height();
		//               pProj->setPosition( colOrigin );
		//               //pProj->onVerticalHit();
		//               pProj->onCollide(m_iEntityType, VERT);
		//               break;
		//
		//            case LEFT:
		//               pProj->setPosition( colOrigin );
		//               //pProj->onHorizontalHit();
		//               pProj->onCollide(m_iEntityType, HORIZ);
		//               break;
		//
		//            case RIGHT:
		//               colOrigin.x -= pProj->Width();
		//               pProj->setPosition( colOrigin );
		//               //pProj->onHorizontalHit();
		//               pProj->onCollide(m_iEntityType, HORIZ);
		//               break;
		//
		//         }

		pProj->setDead();
		pProj->removeTileSpot();
	}


	OrientTBLR resolveCollision( Projectile* pProj,
			Tile* pTile,
			Vector2D* pColOrigin)
	{

		Vector2D origin      (pProj->Position());
		Vector2D direction   (pProj->m_vOldPos2 - origin );
		direction.Normalize();

		int tileX      = Tile::tileContainsCenterColumn( origin.x );
		int tileY      = Tile::tileContainsCenterLine( origin.y );

		// in case projectile and tile are slightly overlapped we need change
		// projectile position to that be into collided tile ( happen when
		// collision type is TOP or LEFT)
		if ( ! m_rTiles . at(tileX) . at(tileY) -> isSolid() )
		{
			// new position is top right of current bbox of projectile
			pProj->setPosition( origin + Vector2D ( pProj->Width(), pProj->Height() ) );

			// updates discreet tile coordinates
			tileX = Tile::tileContainsCenterColumn( pProj->Position().x );
			tileY = Tile::tileContainsCenterLine( pProj->Position().y );
		}

		Ray ray(origin,
				direction,
				tileX,
				tileY,
				Tile::m_fWIDTH,
				Tile::m_fHEIGHT,
				Vector2D(0,0));

		int oldTileX   = 0;
		int oldTileY   = 0;
		int maxLoopNum = 0;

		while(1)
		{
			maxLoopNum++;
			assert ( maxLoopNum < 100);

			oldTileX = ray.CurCellX();
			oldTileY = ray.CurCellY();

			// save the last bound position before ray step
			Vector2D lastBoundPos ( ray.curBoundPosition() );

			ray.step();

			tileX = ray.CurCellX();
			tileY = ray.CurCellY();

			int tilesInWidth = m_rTiles.size();
			int tilesInHeight = m_rTiles[0].size();

			assert (tileX >= 0 && tileX < tilesInWidth && tileY >= 0 && tileY < tilesInHeight );

			Tile* t = m_rTiles . at(tileX) . at(tileY);

			// A void tile has been reached, then now determines
			// collision type and position of impact
			if( ! t->isSolid() )
			{

				*pColOrigin = lastBoundPos;
				OrientTBLR colOri = resolveColType(pProj,
						ray,
						oldTileX,
						oldTileY);

				return colOri;
			}
		} // end while
	}



	OrientTBLR resolveColType(const Projectile* pProj,
			const Ray& ray,
			int oldTileX,
			int oldTileY)
	{
		OrientTBLR colOri = TOP;

		assert ( ! (oldTileX == ray.CurCellX() && oldTileY == ray.CurCellY() ) );

		// if tiles are shifted on X then we'll have 1 or -1 as value
		// which indicates horizontal collision
		if ( oldTileX - ray.CurCellX() != 0 ) //
		{
			if (pProj->Velocity().x < 0)
			{
				colOri = RIGHT;
			}
			else
			{
				colOri = LEFT;
			}
		}
		// else that's vertical collision
		else
		{

			if ( pProj->Velocity().y < 0)
			{
				colOri = TOP;
			}
			else
			{
				colOri = BOTTOM;
			}
		}

		return colOri;
	}

};

#endif // PROJECTILECOLLISIONMODELS_H