#ifndef GFXASSET_H
#define GFXASSET_H

//#define DEBUG_RENDER

#include <vector>
#include <string>
#include <GL/glew.h>

typedef std::vector< std::vector<float> >  Float2DArray;

enum {
	NUM_SKINS = 6
};

enum
{
   TEX_NULL,
   TEX_WHITE,
   TEX_PLAYER_0,
   TEX_PLAYER_1,
   TEX_PLAYER_2,
   TEX_PLAYER_3,
   TEX_PLAYER_4,
   TEX_PLAYER_5,
   TEX_BOT,
   TEX_WEAPONS,
   TEX_POWERUPS,
   TEX_PROJECTILES,
   TEX_PARTICLES,
   TEX_HOUSES_FORWARD_1,
   TEX_HOUSES_FORWARD_2,
   TEX_HOUSES_BACKWARD,
   TEX_SKY_0,
   TEX_SKY_1,
   TEX_SKY_2,
   TEX_SKY_3,
   TEX_CROSSHAIR,
   TEX_TILE_DBG,
   TEX_MAP_DBG,
   TEX_INTERIOR_WALL,
   TEX_TILE_INTERIOR_WALL,
   TEX_WINDOW,
   TEX_WINDOW_2,
   TEX_ROOF,
   TEX_MOUNTAIN,
   TEX_HUD,
   TEX_UI_PANEL,
   TEX_STARTSCREEN,
   TEX_EQUIPMENT,
   TEX_BACKGROUND,
   TEX_MENU_BACKGROUND,
   NUM_TEXTURES
};

enum
{
   MAP_CHAR_STATIONARY,
   MAP_CHAR_WALKFORWARDSET,
   MAP_CHAR_WALKBACKWARDSET,
   MAP_CHAR_JUMP,
   MAP_CHAR_CORPSE,
   MAP_CHAR_ARM,
   MAP_JETPACK,
   MAP_WEAPON_SET,
   MAP_POWERUP_SET,
   MAP_PROJECTILE_SET,
   MAP_FLAMES,
   MAP_CLASSIC_CARTRIDGE,
   MAP_SHOTGUN_CARTRIDGE,
   MAP_RICOCHET,
   MAP_WALL_FRAGMENT,
   MAP_EXPLOSION_YELLOW,
   MAP_EXPLOSION_RED,
   MAP_ROCKET_SMOKE,
   MAP_BLOOD_SPLATTER,
   MAP_BLOOD_CLOUD,
   MAP_GIB_SPLATTER,
   MAP_GIB,
   MAP_HUD_HEALTH,
   NUM_TEX_MAPPINGS
};

class GFXAsset
{
   private:

      struct ImageInfo
      {
         int width;
         int height;
         int format;
         void *data;
      };

      GFXAsset();
      GFXAsset(const GFXAsset &);
      GFXAsset & operator=(const GFXAsset &);

   public:

      static const GFXAsset & Instance() {
      		static GFXAsset gfxAsset;
      		return gfxAsset;
      	}

      void loadAllGfx();
      void loadImages();
      void setTexMappings();

      Float2DArray getTexMapping(int mapId) const;

      void loadTexture (const std::string& filename,
                           int texIdx,
                           int store_format = IMG_RGBA,
                           int flags = 0);

      bool loadPng(ImageInfo& img, const std::string& filename);

      void loadTextureRaw ( int texIdx,
                              int w,
                              int h,
                              int format,
                              const void *data,
                              int store_format,
                              int flags);

      void enableTex (const int texIdx) const;

   private:

      enum { INVALID_TEX = 0 };
      enum { MAX_TEXTURES = 64 };

      enum
      {
         /* Constants: Image Formats */
         IMG_AUTO=-1,   // IMG_AUTO - Lets the engine choose the format.
         IMG_RGB=0,     // IMG_RGB - 8-Bit uncompressed RGB
         IMG_RGBA=1,    // IMG_RGBA - 8-Bit uncompressed RGBA
         IMG_ALPHA=2,   // IMG_ALPHA - 8-Bit uncompressed alpha

         /* Constants: Texture Loading Flags */
         TEXLOAD_NORESAMPLE=1 // Prevents the texture from any resampling
      };

      void convertTexCoord(int numMappings, int xImgDim, int yImgDim,  float coords[][4], Float2DArray& dst);

      static const unsigned char nullTextureData[];
      static const unsigned char whiteTextureData[];

      std::vector<GLuint>           m_cTexIds; ///< ids of textures in opengl
      std::vector<Float2DArray>   m_texMappings;

};

inline void GFXAsset::enableTex(const int texIdx) const
{
   glBindTexture(GL_TEXTURE_2D, m_cTexIds[texIdx]);
}

#endif // GFXASSET_H
