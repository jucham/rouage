#include "RayTracing.h"
#include "Vector2D.h"
#include "Ray.h"
#include <game/server/Tile.h>
#include <game/server/Map.h>

bool RayTracing::canTraceRay(const Vector2D& rayOrigin,
                              const Vector2D& targPos,
                              const Map* pMap)
{

	int curTileX = Tile::tileContainsCenterColumn( rayOrigin.x );
	int curTileY = Tile::tileContainsCenterLine( rayOrigin.y );

	int targTileX = Tile::tileContainsCenterColumn(targPos.x);
	int targTileY = Tile::tileContainsCenterLine( targPos.y );

	Vector2D rayDir = targPos - rayOrigin;
	rayDir.Normalize();

	Ray ray( rayOrigin,
		   rayDir,
		   curTileX,
		   curTileY,
		   Tile::m_fWIDTH,
		   Tile::m_fHEIGHT,
		   Vector2D(0,0));

	while(1)
	{
		ray.step();
		curTileX = ray.CurCellX();
		curTileY = ray.CurCellY();

		Tile* t = pMap->getTiles() . at(curTileX) . at(curTileY);

		if(t->isSolid())
		{
			return false;
		}
		else if (curTileX == targTileX && curTileY == targTileY)
		{
			return true;
		}
	}

	return true;
}
