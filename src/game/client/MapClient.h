#ifndef MAPCLIENT_H_
#define MAPCLIENT_H_

#include <vector>
#include <GL/glew.h>
#include <base/Vector2D.h>
#include <base/DetectPlatform.h>

#if PLATFORM == PLATFORM_MAC
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

class Renderer;
class MapLoader;

//=============================================================================
// TileRender
//=============================================================================
struct TileRender
{
		Vector2D pos;
		bool painted;
		GLuint *m_vertexIndices;
		static float m_TileSize;

		TileRender(float x, float y);
		~TileRender();
		static int getTileIndex(float pos);
};

inline int TileRender::getTileIndex(float pos)
{
	return static_cast<int>(std::floor(pos / m_TileSize));
}

//=============================================================================
// MapRender
//=============================================================================
class MapRender
{

	public:
		typedef std::vector<std::vector<TileRender*> > TileRender2dArray;

		enum
		{
			VBO_BUF_POS, VBO_BUF_TEX, VBO_BUF_COL, NUM_BUFFERS
		};


		MapRender(int drawnTilesOnWidth, int drawnTilesOnHeight);
		~MapRender();
		bool LoadMap(const char *pMapName);
		void Render(const Vector2D& cameraPos);
		void DisableTile(Vector2D pos);

	protected:
		static const int MAX_DRAWN_TILES_ON_WIDTH = 53;

		Renderer          *m_pRenderer;
		TileRender2dArray m_aTiles;
		GLuint            *m_vboBuffers;
		GLuint            *m_vertexIndices;
		char              m_MapName[512];
		char              m_MapFilepath[1024];
		int               m_iMinTilePaintedX;
		int               m_iMaxTilePaintedX;
		int               m_iMinTilePaintedY;
		int               m_iMaxTilePaintedY;

	private:
		void SetFilepath();
		void LoadTilesData(const MapLoader &ml);
		void ComputePaintedTileBounds();
};

#endif /* MAPCLIENT_H_ */
