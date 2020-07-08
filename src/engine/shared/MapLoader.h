#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <vector>
#include <string>
#include <base/FileLoader.h>

///////////////////////////////////////////////////////////////////////////////
///
/// \struct TileData MapLoader.h
/// \brief This struct implements storing of tile data (position, solidity)
///
///////////////////////////////////////////////////////////////////////////////

enum
   {
      PHYSIC_LAYER,
      TEXTURE_LAYER,
      BOT_NAV_LAYER,
      ITEM_LAYER,
      SPAWN_LAYER,
      NUM_LAYERS
   };

struct PhysicData
{
   int x;
   int y;
   int m_iSideSolidity[4];

   PhysicData() : x(0), y(0)
   {
      m_iSideSolidity[0] = 0;
      m_iSideSolidity[1] = 0;
      m_iSideSolidity[2] = 0;
      m_iSideSolidity[3] = 0;
   }
};

struct TexData
{
   int x;
   int y;
   int texId;
   static std::string tileSetFilename;

   TexData() : x(0), y(0) {}
};



struct ItemData
{
   int x;
   int y;

   int itemType;
   int type;

   ItemData() : x(0), y(0), itemType(0), type(0) {}
};

struct SpawnData
{
   int x;
   int y;
};

///////////////////////////////////////////////////////////////////////////////
///
/// \class MapLoader MapLoader.h
/// \brief This class lets game map loading by file .map
///
/// For loading a map, a .map file is opened and its data are recovered to put
/// those in array of data struture nammed TileData. This array lets
/// to store info (position and solidity) of all tiles.
///
///
///////////////////////////////////////////////////////////////////////////////
class MapLoader : public FileHandler
{


public:

   MapLoader(const char* fileName);
   ~MapLoader();

   void ReadHeader();
   void ReadData();
   void ReadPhysicLayer();
   void ReadTextureLayer();
   void ReadItemLayer();
   void ReadSpawnLayer();

   /// \fn const TileData& getTileData(int index) const
   /// \brief Returns data of tile with a given index
   ///
   /// \param index index of tile
   /// \return reference to TileData struct
   ///
   const PhysicData& getPhysicData(int index) const;
   const TexData& getTexData(int index) const;
   const ItemData& getItemData(int index) const;
   const SpawnData& getSpawnData(int index) const;

   /// \fn int NumTilesMaxWidth() const
   /// \brief Returns number of tiles max on width
   ///
   /// \return number of tiles max on width
   ///
   int NumTilesMaxWidth() const;

   /// \fn int NumTilesMaxHeight() const
   /// \brief Returns number of tiles max on height
   ///
   /// \return number of tiles max on height
   ///
   int NumTilesMaxHeight() const;


   int TotalNumTiles(int layer) const;

   bool IsValidMap() const;

private:

   int               m_iNumTilesMaxWidth;          ///<  number of tiles Max on width (
   int               m_iNumTilesMaxHeight;         ///< number of tiles Max on height
   int               m_iTotalNumTiles[NUM_LAYERS]; ///< total number of tile of each layer

   PhysicData*       m_aPhysicData;                  ///< data that represent physic tiles
   TexData*          m_aTexData;
   ItemData*         m_aItemData;
   SpawnData*        m_aSpawnData;

   bool m_ValidMap;

};


#endif // MAPLOADER_H
