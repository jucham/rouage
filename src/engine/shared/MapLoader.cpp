#include <cassert>
#include "MapLoader.h"

std::string TexData::tileSetFilename = "nomapset.png";

MapLoader::MapLoader(const char* fileName) : FileHandler(fileName),
		m_iNumTilesMaxWidth(0),
		m_iNumTilesMaxHeight(0),
		m_iTotalNumTiles(),
		m_aPhysicData(0),
		m_aTexData(0),
		m_aItemData(0),
		m_ValidMap(true)
{}

MapLoader::~MapLoader()
{
	delete[] m_aPhysicData;
	delete[] m_aTexData;
	delete[] m_aItemData;
	delete[] m_aSpawnData;
}


void MapLoader::ReadHeader()
{
	m_File >> m_iNumTilesMaxWidth >> m_iNumTilesMaxHeight;

	const int MIN_NUM_TILES = 5;
	if (m_iNumTilesMaxWidth < MIN_NUM_TILES && m_iNumTilesMaxHeight < MIN_NUM_TILES) {
		printf("Invalid map : the dimensions are too small. Set them greater than %d\n", MIN_NUM_TILES);
		m_ValidMap = false;
	}
}


void MapLoader::ReadData()
{
	ReadPhysicLayer();
	ReadTextureLayer();
	ReadItemLayer();
	ReadSpawnLayer();

	if (m_iTotalNumTiles[SPAWN_LAYER] <= 0) {
		printf("Invalid map : the map contains zero spawn point. [%d]\n", m_iTotalNumTiles[SPAWN_LAYER]);
		m_ValidMap = false;
	}
}

void MapLoader::ReadPhysicLayer()
{
	m_File >> m_iTotalNumTiles[PHYSIC_LAYER];

	m_aPhysicData = new PhysicData[ m_iTotalNumTiles[PHYSIC_LAYER] ];

	for(int i=0; i < m_iTotalNumTiles[PHYSIC_LAYER]; ++i)
	{
		m_File >> m_aPhysicData[i].x >> m_aPhysicData[i].y;

		for(int j=0; j < 4; ++j) { m_File >> m_aPhysicData[i].m_iSideSolidity[j]; }
	}
}

void MapLoader::ReadTextureLayer()
{
	m_File >> m_iTotalNumTiles[TEXTURE_LAYER];

	m_aTexData = new TexData[ m_iTotalNumTiles[TEXTURE_LAYER] ];

	if (m_iTotalNumTiles[TEXTURE_LAYER] > 0) {
		m_File >> TexData::tileSetFilename;

		for(int i=0; i < m_iTotalNumTiles[TEXTURE_LAYER]; ++i)
		{
			m_File >> m_aTexData[i].x >> m_aTexData[i].y >> m_aTexData[i].texId;
		}
	}
}

void MapLoader::ReadItemLayer()
{
	m_File >> m_iTotalNumTiles[ITEM_LAYER];

	m_aItemData = new ItemData[ m_iTotalNumTiles[ITEM_LAYER] ];

	for(int i=0; i < m_iTotalNumTiles[ITEM_LAYER]; ++i)
	{
		m_File >> m_aItemData[i].x >> m_aItemData[i].y;
		m_File >> m_aItemData[i].itemType >> m_aItemData[i].type;
	}
}

void MapLoader::ReadSpawnLayer()
{
	m_File >> m_iTotalNumTiles[SPAWN_LAYER];

	m_aSpawnData = new SpawnData[ m_iTotalNumTiles[SPAWN_LAYER] ];

	for(int i=0; i < m_iTotalNumTiles[SPAWN_LAYER]; ++i)
	{
		m_File >> m_aSpawnData[i].x >> m_aSpawnData[i].y;
	}
}

const PhysicData& MapLoader::getPhysicData(int index) const {return m_aPhysicData[index];}
const TexData& MapLoader::getTexData(int index) const {return m_aTexData[index];}
const ItemData& MapLoader::getItemData(int index) const {return m_aItemData[index];}
const SpawnData& MapLoader::getSpawnData(int index) const {return m_aSpawnData[index];}


int MapLoader::NumTilesMaxWidth() const {return m_iNumTilesMaxWidth;}
int MapLoader::NumTilesMaxHeight() const {return m_iNumTilesMaxHeight;}

int MapLoader::TotalNumTiles(int layer) const {return m_iTotalNumTiles[layer];}

bool MapLoader::IsValidMap() const {return m_ValidMap;}

