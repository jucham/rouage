#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <vector>
#include <cstdio>
#include "TriggerSystem.h"

class Tile;
class GameServer;
class MapLoader;

///////////////////////////////////////////////////////////////////////////////
///
/// \class Map Map.h
/// \brief This implements a game map tile-based
///
/// A map is a set of tiles contained in a 2D array, like a grid. Indexes of
/// array are coordinates of tiles, for instance, tile (8, 12) on grid is
/// reachable by tiles[8][12], with tiles as array. A tile can be solid or not.
///
/// A map can contain several items like : health, armor, weapon, ammo, etc.
/// All those items are triggers (objects which trigger an action) handled by a
/// trigger system.
///
/// Map contains AI logic for bot navigation too, this is navigation graph.
///
///////////////////////////////////////////////////////////////////////////////
class Map
{
	public:
		typedef std::vector<std::vector<Tile*> > TilePtr2dVector;

	private:

		// types of sprite envelops
		enum
		{
			ENVEL_TOP, ENVEL_BOTTOM, ENVEL_LEFT, ENVEL_RIGHT, NUM_ENVEL
		};

		enum
		{
			VBO_BUF_POS, VBO_BUF_TEX, VBO_BUF_COL, NUM_BUFFERS
		};

		TriggerSystem m_TriggerSys;   ///<  trigger system
		TilePtr2dVector m_aTiles;       ///< 2d array of pointer to tile
		std::vector<Vector2D> m_aSpawnPoints;

		char m_MapName[512];
		char m_MapFilepath[1024];

		void LoadTilesData(const MapLoader &ml, GameServer *pGameServer);
		void MakeSolidEnvelop(int maxNumTilesWidth, int maxNumTilesHeight);
		void SetFilepath();

	public:

		Map(GameServer *pGameServer, const char *pMapName = "factory");
		~Map();

		TilePtr2dVector & getTiles()
		{
			return m_aTiles;
		}

		const TilePtr2dVector & getTiles() const
		{
			return m_aTiles;
		}

		TriggerSystem & TriggerSys()
		{
			return m_TriggerSys;
		}

		const std::vector<Vector2D> & SpawnPoints() const;

		const char * GetName() const
		{
			return m_MapName;
		}

};

#endif // MAP_H_INCLUDED
