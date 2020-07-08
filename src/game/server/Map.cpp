#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <sstream>
#include <utility>
#include <algorithm>

#include "Tile.h"
#include "Map.h"
#include "GameServer.h"
#include <engine/shared/MapLoader.h>
#include "HealthGiver.h"
#include "WeaponGiver.h"
#include "EntityEnumerations.h"
#include "ai/PathPlanner.h"
#include "AASLoader.h"
#include <base/utils.h>
#include <base/debug.h>
#include <engine/shared/Common.h>
#include <base/tri_logger.hpp>

void Map::SetFilepath()
{
	const char* path = "data/maps/";
	const char* ext = ".map";
	std::memset(m_MapFilepath, 0, 1024);
	std::strcat(m_MapFilepath, path);
	std::strcat(m_MapFilepath, m_MapName);
	std::strcat(m_MapFilepath, ext);
}

Map::Map(GameServer *pGameServer, const char *mapName) :
																m_TriggerSys(*this)
{
	assert(mapName != 0);

	std::strcpy(m_MapName, mapName);
	SetFilepath();

	MapLoader ml(m_MapFilepath);
	ml.ReadFile();

	if (!ml.IsValidMap()) {
		printf("the map is invalid\n");
		exit(0);
	}

	LoadTilesData(ml, pGameServer);
}

Map::~Map()
{
	// Delete map tiles
	for (int i = 0; i < (int)m_aTiles.size(); ++i)
	{
		for (int j = 0; j < (int)m_aTiles[i].size(); ++j)
		{
			delete m_aTiles[i][j];
		}
		m_aTiles[i].clear();
	}

	m_aTiles.clear();
}

void Map::LoadTilesData(const MapLoader &ml, GameServer *pGameServer)
{
	// curTile of each layer
	int curTile[NUM_LAYERS] = { 0 };

	// Create map tiles from .map file data
	for (int i = 0; i < ml.NumTilesMaxWidth(); ++i)
	{
		m_aTiles.push_back(std::vector<Tile*>());

		for (int j = 0; j < ml.NumTilesMaxHeight(); ++j)
		{
			m_aTiles[i].push_back(new Tile(i * Tile::m_fWIDTH, j * Tile::m_fHEIGHT));

			// --- PROCESS PHYSIC TILE ---

			if (curTile[PHYSIC_LAYER] < ml.TotalNumTiles(PHYSIC_LAYER))
			{
				if ((ml.getPhysicData(curTile[PHYSIC_LAYER]).x == i) && (ml.getPhysicData(curTile[PHYSIC_LAYER]).y == j))
				{
					// Sets tile as solid
					m_aTiles[i][j]->setSolidity(true);

					// A tile is not necessarily solid on all its sides, for
					// instance a tile as piece of the ceiling is only solid on
					// its bottom side.
					if ( ml.getPhysicData(curTile[PHYSIC_LAYER]).m_iSideSolidity[0] == 1 )
						m_aTiles[i][j]->setSideSolidity(Tile::left, true);
					if ( ml.getPhysicData(curTile[PHYSIC_LAYER]).m_iSideSolidity[1] == 1 )
						m_aTiles[i][j]->setSideSolidity(Tile::right, true);
					if ( ml.getPhysicData(curTile[PHYSIC_LAYER]).m_iSideSolidity[2] == 1 )
						m_aTiles[i][j]->setSideSolidity(Tile::top, true);
					if ( ml.getPhysicData(curTile[PHYSIC_LAYER]).m_iSideSolidity[3] == 1 )
						m_aTiles[i][j]->setSideSolidity(Tile::bottom, true);

					curTile[PHYSIC_LAYER]++;
				}
			}

			// --- PROCESS ITEM ---

			if ( curTile[ITEM_LAYER] < ml.TotalNumTiles(ITEM_LAYER) )
			{
				const ItemData& data = ml.getItemData(curTile[ITEM_LAYER]);

				if ( data.x == i && data.y == j )
				{
					Trigger* pTrig = 0;

					switch (data.itemType)
					{

					case IT_POWERUP:
					{
						switch (data.type)
						{
						case PW_HEALTH:
							pTrig = new HealthGiver(pGameServer, &m_TriggerSys, m_aTiles[i][j]->Position());
							break;
						}
						break;
					}

					case IT_WEAPON:
					{
						Vector2D pos(m_aTiles[i][j]->Position() + Vector2D(0, Tile::m_fHEIGHT / 2));
						switch (data.type)
						{
						case WP_PISTOL:
						case WP_ROCKET_LAUNCHER:
						case WP_SHOTGUN:
						case WP_AK47:
							pTrig = new WeaponGiver(pGameServer, &m_TriggerSys, data.type, pos);
							break;

						default:
							printf("wrong weapon type\n");
							assert(false);
						}
						break;
					}

					default:
						printf("wrong item type\n");
						assert(false);
						break;
					}

					assert(pTrig != 0);

					m_TriggerSys.AddTrigger(pTrig);
					m_aTiles[i][j]->SetTrigger(pTrig);

					curTile[ITEM_LAYER]++;
				}
			}

			// --- PROCESS SPAWN POINT ---

			if ( curTile[SPAWN_LAYER] < ml.TotalNumTiles(SPAWN_LAYER) )
			{
				const SpawnData& data = ml.getSpawnData(curTile[SPAWN_LAYER]);

				if ( data.x == i && data.y == j )
				{
					Vector2D pos(i * Tile::m_fWIDTH, j * Tile::m_fHEIGHT);

					m_aSpawnPoints.push_back(pos);
					curTile[SPAWN_LAYER]++;
				}
			}
		}
	}

	MakeSolidEnvelop(ml.NumTilesMaxWidth(), ml.NumTilesMaxHeight());

}

void Map::MakeSolidEnvelop(int maxNumTilesWidth, int maxNumTilesHeight) {

	for (int i = 0; i < maxNumTilesWidth; ++i)
	{
		m_aTiles[i][0]->setSolidity(true);
		m_aTiles[i][0]->setSideSolidity(Tile::top, true);
		m_aTiles[i][maxNumTilesHeight-1]->setSolidity(true);
		m_aTiles[i][maxNumTilesHeight-1]->setSideSolidity(Tile::bottom, true);
	}

	for (int j = 0; j < maxNumTilesHeight; ++j)
	{
		m_aTiles[0][j]->setSolidity(true);
		m_aTiles[0][j]->setSideSolidity(Tile::right, true);
		m_aTiles[maxNumTilesWidth-1][j]->setSolidity(true);
		m_aTiles[maxNumTilesWidth-1][j]->setSideSolidity(Tile::left, true);
	}
}

const std::vector<Vector2D>& Map::SpawnPoints() const
{
	return m_aSpawnPoints;
}

