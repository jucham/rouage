
#include "GFXAsset.h"
#include <cstring>
#include <cassert>
#include <engine/external/pnglite/pnglite.h>
#include <base/debug.h>
#include <game/server/EntityEnumerations.h>

const unsigned char GFXAsset::nullTextureData[] = {
		0xff,0x00,0x00,0xff, 0xff,0x00,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff,
		0xff,0x00,0x00,0xff, 0xff,0x00,0x00,0xff, 0x00,0xff,0x00,0xff, 0x00,0xff,0x00,0xff,
		0x00,0x00,0xff,0xff, 0x00,0x00,0xff,0xff, 0xff,0xff,0x00,0xff, 0xff,0xff,0x00,0xff,
		0x00,0x00,0xff,0xff, 0x00,0x00,0xff,0xff, 0xff,0xff,0x00,0xff, 0xff,0xff,0x00,0xff,
};

const unsigned char GFXAsset::whiteTextureData[] = {
		0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff
};

GFXAsset::GFXAsset() :
		   m_cTexIds(MAX_TEXTURES, 0),
		   m_texMappings()
{
	loadAllGfx();
}

void GFXAsset::loadAllGfx()
{
	loadImages();
	setTexMappings();
}

void GFXAsset::loadImages()
{
	loadTextureRaw(TEX_NULL, 4, 4, IMG_RGBA, nullTextureData, IMG_RGBA, TEXLOAD_NORESAMPLE);
	loadTextureRaw(TEX_WHITE, 4, 4, IMG_RGBA, whiteTextureData, IMG_RGBA, TEXLOAD_NORESAMPLE);

#if defined(DEBUG_RENDER)
	loadTexture( "data/gfx/map/mapset.png",                TEX_MAP_DBG);
	loadTexture( "data/gfx/characters/player-debug.png",         TEX_PLAYER);
	loadTexture( "data/gfx/weapons/weapons-debug.png",           TEX_WEAPONS);
	loadTexture( "data/gfx/weapons/crosshair.png",         TEX_CROSSHAIR);
	loadTexture( "data/gfx/projectiles/projectiles.png",   TEX_PROJECTILES);
	loadTexture( "data/gfx/powerups/powerups.png",         TEX_POWERUPS);
	loadTexture( "data/gfx/particles/particles-debug.png",       TEX_PARTICLES);
	loadTexture( "data/gfx/ui/hud.png",                    TEX_HUD);
	loadTexture( "data/gfx/ui/panel.png",                  TEX_UI_PANEL);
	loadTexture( "data/gfx/ui/startscreen.png",            TEX_STARTSCREEN);
#else
	loadTexture( "data/gfx/map/mapset.png",                TEX_MAP_DBG);

	for (int i = 0; i < NUM_SKINS; ++i) {
		char path[200];
		std::sprintf(path, "data/gfx/characters/player-%d.png", i);
		loadTexture( path, TEX_PLAYER_0 + i);
	}
	loadTexture( "data/gfx/characters/equipment.png",      TEX_EQUIPMENT);

	loadTexture( "data/gfx/weapons/weapons.png",           TEX_WEAPONS);
	loadTexture( "data/gfx/weapons/crosshair.png",         TEX_CROSSHAIR);
	loadTexture( "data/gfx/projectiles/projectiles.png",   TEX_PROJECTILES);
	loadTexture( "data/gfx/powerups/powerups.png",         TEX_POWERUPS);
	loadTexture( "data/gfx/particles/particles.png",       TEX_PARTICLES);
	loadTexture( "data/gfx/ui/hud.png",                    TEX_HUD);
	loadTexture( "data/gfx/ui/panel.png",                  TEX_UI_PANEL);
	loadTexture( "data/gfx/ui/startscreen.png",            TEX_STARTSCREEN);
	loadTexture( "data/gfx/map/background.png",            TEX_BACKGROUND);
	loadTexture( "data/gfx/ui/menu_background.png",        TEX_MENU_BACKGROUND);

#endif
}

void GFXAsset::setTexMappings()
{
	// UV Mapping of image
	// /!\ Take care of mapping order, the pushes must be done in order of the map id enumeration
	// so, first MAPMAP_CHAR_STATIONARY, then MAP_CHAR_WALKFORWARDSET, etc.

	//--------------------------------------------------------------------------
	// --- CHARACTER ---
	//--------------------------------------------------------------------------
	
	// Stationary
	Float2DArray  charStationary;
	charStationary.push_back( std::vector<float>(4, 0.0f) );
	charStationary[0][0] = 0.0f;
	charStationary[0][1] = 0.0f;
	charStationary[0][2] = 1/16.0f;
	charStationary[0][3] = 1.0f;
	m_texMappings.push_back( charStationary );

	// Walk forward
	Float2DArray  charWalkForwardSet;
	int size = 11;
	for (int i = 0; i < size; ++i)
	{
		charWalkForwardSet.push_back( std::vector<float>(4, 0.0f) );
		charWalkForwardSet[i][0] = i* 1/16.0;
		charWalkForwardSet[i][1] = 0.0f;
		charWalkForwardSet[i][2] = (i+1) * 1/16.0;
		charWalkForwardSet[i][3] = 1.0f;
	}
	m_texMappings.push_back( charWalkForwardSet );

	// Walk backward
	Float2DArray  charWalkBackwardSet( charWalkForwardSet.size() );
	Float2DArray::iterator it;
	int k = charWalkForwardSet.size()-1;
	for(  it = charWalkBackwardSet.begin();
			it != charWalkBackwardSet.end();
			++it)
	{
		*it = charWalkForwardSet[k--];
	}
	m_texMappings.push_back( charWalkBackwardSet );


	// Jump
	Float2DArray  charJump;
	int offset = size;
	charJump.push_back( std::vector<float>(4, 0.0f) );
	charJump[0][0] = offset * 1/16.0;
	charJump[0][1] = 0.0f;
	charJump[0][2] = (offset+1) * 1/16.0;
	charJump[0][3] = 1.0f;
	m_texMappings.push_back( charJump );

	// Corpse
	Float2DArray  charCorpse;
	offset++;
	float corpseMap[] = { offset*1/16.0f,  0.0f,  (offset+1)*1/16.f,  1.0f}; //TODO declare like that as soon as possible
	charCorpse.push_back( std::vector<float>(corpseMap, corpseMap+4) );
	m_texMappings.push_back( charCorpse );

	// Arm
	Float2DArray  charArm;
	offset++;
	charArm.push_back( std::vector<float>(4, 0.0f) );
	charArm[0][0] = offset * 1/16.0;
	charArm[0][1] = 0.0f;
	charArm[0][2] = (offset + 0.70) * 1/16.0;
	charArm[0][3] = 0.15f;
	m_texMappings.push_back( charArm );

	// Jetpack
	Float2DArray  jetpack;
	jetpack.push_back( std::vector<float>(4, 0.0f) );
	jetpack[0][0] = 0.0f;
	jetpack[0][1] = 0.0f;
	jetpack[0][2] = 184/512.0f;
	jetpack[0][3] = 187/512.0f;
	m_texMappings.push_back( jetpack );

	//--------------------------------------------------------------------------
	// --- WEAPONS ---
	//--------------------------------------------------------------------------
	Float2DArray  weaponSet;
	float weaponCoords[][4] =
	{
			{0, 0, 122, 60}, // pistol
			{0, 206, 250, 330}, // rocket launcher
			{0, 60, 231, 125}, // shotgun
			{0, 136, 211, 198} // AK47

	};
	convertTexCoord(4, 256, 512, weaponCoords, weaponSet);
	m_texMappings.push_back( weaponSet );

	//--------------------------------------------------------------------------
	// --- POWERUPS ---
	//--------------------------------------------------------------------------
	Float2DArray  powerupSet;
	float powerupCoords[][4] =
	{
			{0, 0, 64, 64} // health
	};
	convertTexCoord(1, 64, 64, powerupCoords, powerupSet);
	m_texMappings.push_back( powerupSet );

	//--------------------------------------------------------------------------
	// --- PROJS ---
	//--------------------------------------------------------------------------
	Float2DArray  projectileSet;
	float projCoords[][4] =
	{
			{0, 0, 50, 16}, // bullet
			{0, 14, 82, 49} // rocket
	};
	convertTexCoord(2, 128, 64, projCoords, projectileSet);
	m_texMappings.push_back( projectileSet );


	//--------------------------------------------------------------------------
	// --- PARTICLES ---
	//--------------------------------------------------------------------------

	//
	// FLAMES
	//
	Float2DArray  flames;
	float flameMap[][4] =
	{
			{0, 0, 123, 30}, //
			{0, 30, 123, 63}, //
			{0, 63, 123, 93} //
	};
	convertTexCoord(3, 512, 512, flameMap, flames);
	m_texMappings.push_back( flames );

	//
	// CLASSIC CARTRIDGES
	//
	Float2DArray  classicCart;
	float classicCartMap[][4] =
	{
			{16, 345, 49, 356}
	};
	convertTexCoord(1, 512, 512, classicCartMap, classicCart);
	m_texMappings.push_back( classicCart );

	//
	// SHOTGUN CARTRIDGES
	//
	Float2DArray  shotgunCart;
	float shotgunCartMap[][4] =
	{
			{18, 367, 50, 378}
	};
	convertTexCoord(1, 512, 512, shotgunCartMap, shotgunCart);
	m_texMappings.push_back( shotgunCart );

	//
	// RICOCHET
	//
	Float2DArray  ricochet;
	float ricochetMap[][4] =
	{
			{0, 95, 125, 195}
	};
	convertTexCoord(1, 512, 512, ricochetMap, ricochet);
	m_texMappings.push_back( ricochet );

	//
	// WALL FRAGMENT
	//
	Float2DArray  wallFragment;
	float wallFragmentMap[][4] =
	{
			{21, 391, 43, 412}
	};
	convertTexCoord(1, 512, 512, wallFragmentMap, wallFragment);
	m_texMappings.push_back( wallFragment );

	//
	// EXPLOSION YELLOW
	//
	Float2DArray  expYellow;
	float expYellowMap[][4] =
	{
			{149, 3, 274, 127}
	};
	convertTexCoord(1, 512, 512, expYellowMap, expYellow);
	m_texMappings.push_back( expYellow );

	//
	// EXPLOSION RED
	//
	Float2DArray  expRed;
	float expRedMap[][4] =
	{
			{149, 135, 286, 268}
	};
	convertTexCoord(1, 512, 512, expRedMap, expRed);
	m_texMappings.push_back( expRed );

	//
	// ROCKET SMOKE
	//
	Float2DArray  rocketSmoke;
	float rocketSmokeMap[][4] =
	{
			{0, 206, 131, 334}
	};
	convertTexCoord(1, 512, 512, rocketSmokeMap, rocketSmoke);
	m_texMappings.push_back( rocketSmoke );

	//
	// BLOOD SPLATTER
	//
	Float2DArray  bloodSplatters;
	float bloodSplattersMap[][4] =
	{
			{312, 0, 347, 27}, // 1
			{313, 32, 347, 58}, // 2
			{312, 70, 346, 88} // 3
	};
	convertTexCoord(3, 512, 512, bloodSplattersMap, bloodSplatters);
	m_texMappings.push_back( bloodSplatters );

	//
	// BLOOD CLOUD
	//
	Float2DArray  bloodClouds;
	float bloodCloudsMap[][4] =
	{
			{367, 6, 430, 67}, //
			{438, 6, 500, 67}
	};
	convertTexCoord(2, 512, 512, bloodCloudsMap, bloodClouds);
	m_texMappings.push_back( bloodClouds );

	//
	// GIB SPLATTER
	//
	Float2DArray  gibSplatter;
	float gibSplatterMap[][4] =
	{
			{368, 74, 504, 214}
	};
	convertTexCoord(1, 512, 512, gibSplatterMap, gibSplatter);
	m_texMappings.push_back( gibSplatter );


	//
	// GIB
	//
	Float2DArray  gib;
	float gibMap[][4] =
	{
			{382, 224, 409, 262}, //head
			{407, 261, 439, 294}, // torso
			{439, 227, 456, 247},// arm
			{460, 229, 473, 246}, // forward arm
			{415, 226, 435, 249}, // high leg
			{452, 261, 475, 284} // low leg
	};

	convertTexCoord(6, 512, 512, gibMap, gib);
	m_texMappings.push_back( gib );

	//--------------------------------------------------------------------------
	// --- HUD ---
	//--------------------------------------------------------------------------
	Float2DArray  hud;
	float hudMap[][4] =
	{
			{0, 0, 100, 95} // health
	};

	convertTexCoord(1, 128, 128, hudMap, hud);
	m_texMappings.push_back( hud );

}

//FIXME should return a const reference
Float2DArray GFXAsset::getTexMapping(int mapId) const
{
	assert ( mapId >= 0 && mapId <= NUM_TEX_MAPPINGS );
	return m_texMappings[mapId];
}

bool GFXAsset::loadPng(ImageInfo& img, const std::string& filename)
{
	unsigned char *buffer;
	png_t png;

	/* open file for reading */
	png_init(0,0);

	if(png_open_file(&png, filename.c_str()) != PNG_NO_ERROR)
	{
#if DBG
		dbg << "open fails" << std::endl;
#endif
		return false;
	}

	if(png.depth != 8 || (png.color_type != PNG_TRUECOLOR && png.color_type != PNG_TRUECOLOR_ALPHA))
	{
#if DBG
		dbg << "wrong png format" << std::endl;
#endif
		png_close_file(&png);
		return false;
	}

	buffer = new unsigned char[png.width * png.height * png.bpp];
	png_get_data(&png, buffer);
	png_close_file(&png);

	img.width = png.width;
	img.height = png.height;
	if(png.color_type == PNG_TRUECOLOR)
		img.format = IMG_RGB;
	else if(png.color_type == PNG_TRUECOLOR_ALPHA)
		img.format = IMG_RGBA;
	img.data = buffer;

	return true;
}

void GFXAsset::loadTextureRaw( int texIdx,
		int w,
		int h,
		int format,
		const void *data,
		int store_format,
		int flags)
{
	unsigned char *texdata = (unsigned char *)data;
	int oglformat = 0;
	int store_oglformat = 0;

	/* grab texture */
	oglformat = GL_RGBA;
	if(format == IMG_RGB)
		oglformat = GL_RGB;
	else if(format == IMG_ALPHA)
		oglformat = GL_ALPHA;


	store_oglformat = GL_RGBA;
	if(store_format == IMG_RGB)
		store_oglformat = GL_RGB;
	else if(store_format == IMG_ALPHA)
		store_oglformat = GL_ALPHA;

	glGenTextures(1, &m_cTexIds[ texIdx ] );
	glBindTexture(GL_TEXTURE_2D, m_cTexIds[ texIdx ]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, store_oglformat, w, h, oglformat, GL_UNSIGNED_BYTE, texdata);
}

void GFXAsset::loadTexture(const std::string& filename,
		int texIdx,
		int store_format,
		int flags)
{
	ImageInfo img;

	if(loadPng(img, filename))
	{
		if (store_format == IMG_AUTO)
			store_format = img.format;

		loadTextureRaw(texIdx, img.width, img.height, img.format, img.data, store_format, flags);
		delete[] ( (unsigned char*)img.data );
	}
	else
	{
		m_cTexIds[ texIdx ] = m_cTexIds[ TEX_NULL ];
	}

}


void GFXAsset::convertTexCoord(int numMappings, int xImgDim, int yImgDim, float coords[][4], Float2DArray& dst)
{
	float xdim = xImgDim;
	float ydim = yImgDim;

	for (int i=0; i<numMappings; i++)
	{
		// convert x coordinates
		coords[i][0] /= xdim;
		coords[i][2] /= xdim;

		// convert y coordinates
		coords[i][1] /= ydim;
		coords[i][3] /= ydim;

		dst.push_back( std::vector<float>(coords[i], coords[i]+4) );
	}
}

