#include "MapClient.h"
#include "Renderer.h"
#include <engine/shared/MapLoader.h>
#include <engine/client/gfx/GFXAsset.h>
#include <engine/client/gfx/VertexBufferObject.h>
#include <engine/client/Window.h>
#include <engine/shared/Common.h>
#include <base/tri_logger.hpp>
#include <game/server/Tile.h>

#define BUFFER_OFFSET_INT(offset) ((GLuint*)0+(offset))

//=============================================================================
// TileRender
//=============================================================================

float TileRender::m_TileSize = 0.0f;

TileRender::TileRender(float x, float y) :
				pos(x, y), painted(false), m_vertexIndices(0)
{
	m_vertexIndices = new GLuint[4];
	for (int i = 0; i < 4; i++)
		m_vertexIndices[i] = i;
}

TileRender::~TileRender()
{
	delete[] m_vertexIndices;
}

//=============================================================================
// MapRender
//=============================================================================

//const char * MapRender::m_MAP_FOLDER = "data/maps/";

MapRender::MapRender(int drawnTilesOnWidth, int drawnTilesOnHeight) :
m_aTiles(),
m_vboBuffers(0),
m_vertexIndices(0),
m_iMinTilePaintedX(0),
m_iMaxTilePaintedX(0),
m_iMinTilePaintedY(0),
m_iMaxTilePaintedY(0)
{
	m_vboBuffers = new GLuint[NUM_BUFFERS];
	int maxTilesRendered = MAX_DRAWN_TILES_ON_WIDTH * drawnTilesOnHeight;
	m_vertexIndices = new GLuint [ 4 * maxTilesRendered ];
}

MapRender::~MapRender()
{
	delete[] m_vboBuffers;
	delete[] m_vertexIndices;

	for (int i = 0; i < (int)m_aTiles.size(); i++)
		for (int j = 0; j < (int)m_aTiles[0].size(); j++)
			delete m_aTiles[i][j];
}

void MapRender::LoadTilesData(const MapLoader &ml)
{
	// curTile of each layer
	int curTile[NUM_LAYERS] = { 0 };

	// 8 floats to make a position or coord texture quad
	const int QUAD_FLOAT_NUM = 8;
	const int POS_SIZE = QUAD_FLOAT_NUM * ml.TotalNumTiles(TEXTURE_LAYER);
	const int TEX_SIZE = QUAD_FLOAT_NUM * ml.TotalNumTiles(TEXTURE_LAYER);

	GLfloat *vboPosData = new GLfloat[POS_SIZE];
	GLfloat *vboTexData = new GLfloat[TEX_SIZE];

	// Create map tiles from .map file data
	for (int i = 0; i < ml.NumTilesMaxWidth(); ++i)
	{
		m_aTiles.push_back(std::vector<TileRender*>());

		for (int j = 0; j < ml.NumTilesMaxHeight(); ++j)
		{
			m_aTiles[i].push_back(new TileRender(i * TileRender::m_TileSize, j * TileRender::m_TileSize));

			// --- Texture tile processing ---
			if ( curTile[TEXTURE_LAYER] < ml.TotalNumTiles(TEXTURE_LAYER) )
			{
				const TexData& data = ml.getTexData(curTile[TEXTURE_LAYER]);

				if ( data.x == i && data.y == j )
				{
					m_aTiles[i][j]->painted = true;

					int k = QUAD_FLOAT_NUM * curTile[TEXTURE_LAYER];

					//configure les indices de vertex pour le tile
					// si k=8 alors le premier indice est k/2 soit 4
					for (int m = 0; m < 4; m++)
					{
						m_aTiles[i][j]->m_vertexIndices[m] = k / 2 + m;
					}

					// Set position
					float x = m_aTiles[i][j]->pos.x;
					float y = m_aTiles[i][j]->pos.y;
					float w = TileRender::m_TileSize;
					float h = TileRender::m_TileSize;

					vboPosData[k] = x + w;
					vboPosData[k + 1] = y;
					vboPosData[k + 2] = x + w;
					vboPosData[k + 3] = y + h;
					vboPosData[k + 4] = x;
					vboPosData[k + 5] = y + h;
					vboPosData[k + 6] = x;
					vboPosData[k + 7] = y;

					// Set texture coords
					float tile_pixelsize = 1024 / 32.0f;
					float final_tilesize = TileRender::m_TileSize;
					float final_tilesize_scale = final_tilesize / tile_pixelsize;

					float texsize = 1024.0f;
					float frac = (1.25f / texsize) * (1 / final_tilesize_scale);
					float nudge = (0.5f / texsize) * (1 / final_tilesize_scale);

					int tx = data.texId % 16;
					int ty = data.texId / 16;
					int px0 = tx * (1024 / 16);
					int py0 = ty * (1024 / 16);
					int px1 = (tx + 1) * (1024 / 16) - 1;
					int py1 = (ty + 1) * (1024 / 16) - 1;

					float u0 = nudge + px0 / texsize + frac; // nudge + frac
					float v0 = nudge + py0 / texsize + frac;
					float u1 = nudge + px1 / texsize - frac; // nudge + (texsize-1)/texsize - frac
					float v1 = nudge + py1 / texsize - frac;

					vboTexData[k] = u1;
					vboTexData[k + 1] = v1;
					vboTexData[k + 2] = u1;
					vboTexData[k + 3] = v0;
					vboTexData[k + 4] = u0;
					vboTexData[k + 5] = v0;
					vboTexData[k + 6] = u0;
					vboTexData[k + 7] = v1;

					curTile[TEXTURE_LAYER]++;
				}
			}
		}
	}

	// === Copying tile info in GPU ===

	const int POS_BYTE_SIZE = POS_SIZE * sizeof(float);
	const int TEX_BYTE_SIZE = TEX_SIZE * sizeof(float);

	// Create vbo buffers
	glGenBuffersARB(NUM_BUFFERS, m_vboBuffers);

	// set position
	glBindBufferARB(GL_ARRAY_BUFFER, m_vboBuffers[VBO_BUF_POS]);
	glBufferDataARB(GL_ARRAY_BUFFER, POS_BYTE_SIZE, vboPosData, GL_STATIC_DRAW);

	// set texture mapping
	glBindBufferARB(GL_ARRAY_BUFFER, m_vboBuffers[VBO_BUF_TEX]);
	glBufferDataARB(GL_ARRAY_BUFFER, TEX_BYTE_SIZE, vboTexData, GL_STATIC_DRAW);

	//set color
	const int COLOR_SIZE = 16 * ml.TotalNumTiles(TEXTURE_LAYER);
	float *vboColData = new float[COLOR_SIZE];
	for (int i = 0; i < COLOR_SIZE; i++)
	{
		vboColData[i] = 1.0f;
	}
	const int COLOR_BYTE_SIZE = COLOR_SIZE * sizeof(float);
	glBindBufferARB(GL_ARRAY_BUFFER, m_vboBuffers[VBO_BUF_COL]);
	glBufferDataARB(GL_ARRAY_BUFFER, COLOR_BYTE_SIZE, vboColData, GL_STATIC_DRAW);

	VertexBufferObject::UnbindBuffer();

	delete[] vboPosData;
	delete[] vboTexData;
	delete[] vboColData;
}

void MapRender::ComputePaintedTileBounds()
{
	// Computes 4 limit painted tiles
	int minX = m_aTiles.size();
	int maxX = 0;
	int minY = m_aTiles[0].size();
	int maxY = 0;
	for (int i = 0; i < (int)m_aTiles.size(); i++)
	{
		for (int j = 0; j < (int)m_aTiles[0].size(); j++)
		{
			if ( m_aTiles[i][j]->painted )
			{
				if ( i < minX ) minX = i;
				if ( i > maxX ) maxX = i;
				if ( j < minY ) minY = j;
				if ( j > maxY ) maxY = j;
			}
		}
	}

	m_iMinTilePaintedX = minX;
	m_iMaxTilePaintedX = maxX;
	m_iMinTilePaintedY = minY;
	m_iMaxTilePaintedY = maxY;
}

void MapRender::Render(const Vector2D& cameraPos)
{
	Vector2D camBL(cameraPos);
	Vector2D camTR(cameraPos + Window::FrameDims());

	// tile coords of camera Bottom Left and top right
	// render more tiles to handle widescreen
	int camBLX = TileRender::getTileIndex(camBL.x) - 7;
	int camBLY = TileRender::getTileIndex(camBL.y);
	int camTRX = TileRender::getTileIndex(camTR.x) + 7;
	int camTRY = TileRender::getTileIndex(camTR.y);

	//check that index of tiles don't go over tile painted limits and fix if needed
	if ( camBLX < m_iMinTilePaintedX )
		camBLX = m_iMinTilePaintedX;
	if ( camBLX > m_iMaxTilePaintedX )
		camBLX = m_iMaxTilePaintedX;
	if ( camBLY < m_iMinTilePaintedY )
		camBLY = m_iMinTilePaintedY;
	if ( camBLY > m_iMaxTilePaintedY )
		camBLY = m_iMaxTilePaintedY;
	if ( camTRX < m_iMinTilePaintedX )
		camTRX = m_iMinTilePaintedX;
	if ( camTRX > m_iMaxTilePaintedX )
		camTRX = m_iMaxTilePaintedX;
	if ( camTRY < m_iMinTilePaintedY )
		camTRY = m_iMinTilePaintedY;
	if ( camTRY > m_iMaxTilePaintedY )
		camTRY = m_iMaxTilePaintedY;

	// Constructs indices arrays, choose the goods vertices to draw
	int tileDrawnCount = 0;

	// loop on tiles in a specific window determined by drawing limits
	for (int i = camBLX; i <= camTRX; i++)
	{
		for (int j = camBLY; j <= camTRY; j++)
		{
			if ( m_aTiles[i][j]->painted )
			{
				for (int k = 0; k < 4; k++)
				{
					int m = 4 * tileDrawnCount;
					m_vertexIndices[m+k] = m_aTiles[i][j]->m_vertexIndices[k];
				}
				tileDrawnCount++;
			}
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER, m_vboBuffers[VBO_BUF_POS]);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glBindBufferARB(GL_ARRAY_BUFFER, m_vboBuffers[VBO_BUF_TEX]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glBindBufferARB(GL_ARRAY_BUFFER, m_vboBuffers[VBO_BUF_COL]);
	glColorPointer(4, GL_FLOAT, 0, 0);

	// create indexes of vertice buffer
	int numIndices = 4 * tileDrawnCount;
	GLuint bufInd;
	glGenBuffersARB(1, &bufInd);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, bufInd);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), m_vertexIndices, GL_STATIC_DRAW);
	VertexBufferObject::UnbindBuffer();

	glEnable(GL_TEXTURE_2D);
	GFXAsset::Instance().enableTex(TEX_MAP_DBG);

	const int NUM_DRAW_CALLS = 4;
	int numQuadByDrawCall[NUM_DRAW_CALLS];
	for (int i = 0; i < NUM_DRAW_CALLS; i++) {
		numQuadByDrawCall[i] = tileDrawnCount / NUM_DRAW_CALLS;
	}

	// add the remaining quads for the first draw call
	int restQuadToDraw = tileDrawnCount % NUM_DRAW_CALLS;
	if ( restQuadToDraw > 0 ) {
		numQuadByDrawCall[0] += restQuadToDraw;
	}

	int numIndicesByDrawCall[NUM_DRAW_CALLS];
	for (int i = 0; i < NUM_DRAW_CALLS; i++) {
		numIndicesByDrawCall[i] = 4 * numQuadByDrawCall[i];
	}

#define RANGE_DRAW 0

#if RANGE_DRAW
	int offset = 0;
#else
	int offset = 1;
#endif

	for (int i = 0; i < NUM_DRAW_CALLS; i++)
	{
#if RANGE_DRAW
		glDrawRangeElements(GL_QUADS,
				offset,
				offset+numIndicesByDrawCall[i]-1,
				numIndicesByDrawCall[i],
				GL_UNSIGNED_INT,
				BUFFER_OFFSET_INT(offset));
#else
		glDrawElements(GL_QUADS, numIndicesByDrawCall[i], GL_UNSIGNED_INT, BUFFER_OFFSET_INT(offset-1));
#endif

		offset += numIndicesByDrawCall[i];
	}

	glDisable(GL_TEXTURE_2D);
	glDeleteBuffersARB(1, &bufInd);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

bool MapRender::LoadMap(const char *pMapName)
{
	assert(pMapName != 0);

	std::strcpy(m_MapName, pMapName);
	SetFilepath();

	printf("load the map %s\n", m_MapFilepath);

	MapLoader ml(m_MapFilepath);
	ml.ReadFile();

	if (!ml.IsValidMap()) {
		return false;
	}

	LoadTilesData(ml);
	ComputePaintedTileBounds();

	return true;
}

void MapRender::SetFilepath()
{
	const char* path = "data/maps/";
	const char* ext = ".map";
	std::memset(m_MapFilepath, 0, 1024);
	std::strcat(m_MapFilepath, path);
	std::strcat(m_MapFilepath, m_MapName);
	std::strcat(m_MapFilepath, ext);
}

void MapRender::DisableTile(Vector2D pos) {

	int col = Tile::tileContainsCenterColumn( pos.x );
	int lin = Tile::tileContainsCenterLine( pos.y );
	int tilesInWidth = static_cast<int> (m_aTiles.size());
	int tilesInHeight = static_cast<int> (m_aTiles[0].size());

	assert( ! (col < 0 || lin < 0 || col > tilesInWidth-1 || lin > tilesInHeight-1) );

	m_aTiles[col][lin]->painted = false;
}
