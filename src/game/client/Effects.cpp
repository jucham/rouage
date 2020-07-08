#include "Effects.h"

#include <game/client/GameClient.h>
#include <game/server/Weapon.h>
#include <game/server/Bullet.h>
#include <game/server/Tile.h>
#include <game/server/Character.h>

#include <engine/client/gfx/GFXAsset.h>
#include <engine/client/gfx/VBOSprite.h>
#include <engine/client/sfx/SFXAsset.h>
#include "ParticleManager.h"
#include "Giblet.h"

Particle* Effects::ThrowCartridge(int weaponType, Vector2D weaponPos, Vector2D weaponDir, float weaponAngle, float weaponEjectOffset)
{
   int         lifeSpan          = 10000;
   float       size              =  Tile::m_fWIDTH*0.27f;
   Vector2D    sprSize           (size, size/3.0f);
   float       curAngle          = weaponAngle;
   float       angleSpeed        = HalfPi/8;
   int         startFlyingTime   = Time::ms();
   float       g                 = Tile::m_fHEIGHT * 0.8f;
   Vector2D    v0                (0, RandInRange(Tile::m_fHEIGHT*0.2f , Tile::m_fHEIGHT*0.3f));
   bool        mustBounce        = true;
   float       bounceCoef        = 0.5f;
   bool        mustBeCollided    = true;

   float startRgb[] = {1.0f, 1.0f, 1.0f, 1.0f};
   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
   std::vector<float> startColor (startRgb, startRgb+4 );
   std::vector<float> endColor (endRgb, endRgb+4 );

   Vector2D boxSize(Bullet::m_fWIDTH, Bullet::m_fHEIGHT );

   int firstSndId = 0;
   int lastSndId = 0;

   if (weaponType == WP_AK47)
   {

      firstSndId = SND_CLASSIC_CARTRIDGE_1;
      lastSndId = SND_CLASSIC_CARTRIDGE_3;
   }
   else
   {
      firstSndId = SND_SHOTGUN_CARTRIDGE_2;
      lastSndId = SND_SHOTGUN_CARTRIDGE_4;
   }

   if (weaponDir.x < 0)
      v0.x = RandInRange( Tile::m_fHEIGHT*0.1f, Tile::m_fHEIGHT*0.2f);
   else
      v0.x = -RandInRange( Tile::m_fHEIGHT*0.1f, Tile::m_fHEIGHT*0.2f);

   Vector2D pos(weaponPos + weaponDir * weaponEjectOffset);

   Particle* pPart = new Particle (
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprSize,
                                    sprSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize,
                                    firstSndId,
                                    lastSndId);

   if (weaponType == WP_AK47)
   {
      Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_CLASSIC_CARTRIDGE);
      pPart->getSprite().setTexMapping( mapping[0] );
   }
   else
   {
      Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_SHOTGUN_CARTRIDGE);
      pPart->getSprite().setTexMapping( mapping[0] );
   }

   return pPart;
}

Particle* Effects::ShotSmoke(Vector2D weaponPos, Vector2D weaponDir)
{
   int      lifeSpan = 1500;
   Vector2D sprStartSize ( Tile::m_fWIDTH, Tile::m_fHEIGHT);
   Vector2D sprEndSize ( Tile::m_fWIDTH*3, Tile::m_fHEIGHT*3);

   float    curAngle          = 0.0f;
   float    angleSpeed        = 0;
   int      startFlyingTime   = 0;
   float    g                 = 0.0f;
   Vector2D v0                ( 0.0f,  0.0f);
   bool     mustBounce        = false;
   float    bounceCoef        = 0.0f;
   bool     mustBeCollided    = false;

   float startRgb[] = {1.0f, 1.0f, 1.0f, 0.25f};
   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
   std::vector<float> startColor (startRgb, startRgb+4 );
   std::vector<float> endColor (endRgb, endRgb+4 );

   Vector2D boxSize( 1.0f, 1.0f );
   Vector2D pos( weaponPos );

   weaponDir *= Tile::m_fWIDTH*2;
   pos += weaponDir;

   Particle* pPart = new Particle (
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize
                                    );
   Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_ROCKET_SMOKE);
   pPart->getSprite().setTexMapping( mapping[0] );

   return pPart;
}


ParticleList Effects::RocketExplosion(Vector2D pos)
{

   // reference size
   float rs = Tile::m_fWIDTH;

   int      lifeSpan = 0;
   Vector2D sprStartSize;
   Vector2D sprEndSize;

   float    curAngle          = 0.0f;
   float    angleSpeed        = 0.0f;
   int      startFlyingTime   = 0;
   float    g                 = 0.0f;
   Vector2D v0;
   bool     mustBounce        = false;
   float    bounceCoef        = 0.0f;
   bool     mustBeCollided    = false;


   float startRgb[] = {1.0f, 1.0f, 1.0f, 1.0f};
   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
   std::vector<float> startColor (startRgb, startRgb+4 );
   std::vector<float> endColor (endRgb, endRgb+4 );

   Vector2D boxSize;
   ParticleList partList;

   ////////////////////////////////////////////////////////////////////////////


   // --- smoke clouds ---
   lifeSpan = RandInt(1400,1600);
   sprStartSize.x = sprStartSize.y  = rs*1.2f;
   sprEndSize     = sprStartSize*2.5f;

   curAngle          = 0.0f;
   angleSpeed        = HalfPi/200;

   for(int i=0; i<9; i++)
   {
      Vector2D v0 ( std::cos(i*QuarterPi),  std::sin(i*QuarterPi) );
      // a cloud to center of explosion
      if ( i==0 ) { v0.Zero(); }

      float length = Tile::m_fHEIGHT * RandInRange(0.01f, 0.03f);
      v0 *= length;

      Particle* pPart = new Particle(
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize);
      Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_ROCKET_SMOKE);
      pPart->getSprite().setTexMapping(mapping[0]);
      pPart->getSprite().setCentered();

      partList.push_back(pPart);

   }


    // --- red clouds ---
   lifeSpan = RandInt(550, 650);
   sprStartSize.x = sprStartSize.y  = rs*1.8f;
   sprEndSize = sprStartSize * 2;


   curAngle = RandInRange(0, 2*Pi); // todo : put rand value
   angleSpeed = 0.0f;

   Particle* pPart = new Particle(
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize);
      Float2DArray map1 = GFXAsset::Instance().getTexMapping(MAP_EXPLOSION_RED);
      pPart->getSprite().setTexMapping(map1[0]);
      pPart->getSprite().setCentered();

      partList.push_back(pPart);

   // --- yellow clouds ---
   lifeSpan = RandInt(350, 450);
   sprStartSize.x = sprStartSize.y  = rs*1.3;
   sprEndSize = sprStartSize * 2;

   curAngle = RandInRange(0, 2*Pi); // todo : put rand value
   angleSpeed = 0.0f;

   pPart = new Particle(
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize
                                    //pSprite
                                    );
      Float2DArray map2 = GFXAsset::Instance().getTexMapping(MAP_EXPLOSION_YELLOW);
      pPart->getSprite().setTexMapping(map2[0]);
      pPart->getSprite().setCentered();
      partList.push_back(pPart);

   return partList;
}

Particle* Effects::RocketSmokeCloud(Vector2D pos)
{
   int      lifeSpan = 2000;
   Vector2D sprStartSize ( Tile::m_fWIDTH, Tile::m_fHEIGHT);
   Vector2D sprEndSize ( Tile::m_fWIDTH*4, Tile::m_fHEIGHT*4);

   float    curAngle          = RandInRange(0, 2*Pi);
   float    angleSpeed        = 0.0f;
   int      startFlyingTime   = 0;
   float    g                 = 0.0f;
   Vector2D v0                ( 0.0f,  0.0f);
   bool     mustBounce        = false;
   float    bounceCoef        = 0.0f;
   bool     mustBeCollided    = false;

   float startRgb[] = {1.0f, 1.0f, 1.0f, 1.0f};
   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
   std::vector<float> startColor (startRgb, startRgb+4 );
   std::vector<float> endColor (endRgb, endRgb+4 );

   Vector2D boxSize( Tile::m_fWIDTH, Tile::m_fWIDTH );

   Particle* pPart = new Particle (
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize
                                    );
      Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_ROCKET_SMOKE);
      pPart->getSprite().setTexMapping(mapping[0]);
      pPart->getSprite().setCentered();

   return pPart;
}

Particle* Effects::JetpackSmokeCloud(Vector2D pos)
{
   int      lifeSpan = 500;
   Vector2D sprStartSize ( Tile::m_fWIDTH/2, Tile::m_fHEIGHT/2);
   Vector2D sprEndSize ( Tile::m_fWIDTH*2, Tile::m_fHEIGHT*2);

   float    curAngle          = RandInRange(0, 2*Pi);
   float    angleSpeed        = 0.0f;
   int      startFlyingTime   = 0;
   float    g                 = 0.0f;
   Vector2D v0                ( 0.0f,  0.0f);
   bool     mustBounce        = false;
   float    bounceCoef        = 0.0f;
   bool     mustBeCollided    = false;

   float startRgb[] = {1.0f, 1.0f, 1.0f, 1.0f};
   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
   std::vector<float> startColor (startRgb, startRgb+4 );
   std::vector<float> endColor (endRgb, endRgb+4 );

   Vector2D boxSize( Tile::m_fWIDTH, Tile::m_fWIDTH );

   Particle* pPart = new Particle (
                                    lifeSpan,
                                    TEX_NULL,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize
                                    );
      Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_ROCKET_SMOKE);
      pPart->getSprite().setTexMapping(mapping[0]);
      pPart->getSprite().setCentered();

   return pPart;
}

Particle* Effects::ShotFlame(Vector2D weaponPos, Vector2D weaponDir, float weaponAngle, float projCannonOffset, float flameDist)
{
   int      lifeSpan = 50;
   Vector2D sprSize ( Tile::m_fWIDTH*2.3f, Tile::m_fHEIGHT*0.6f);
   float    curAngle          = weaponAngle;
   float    angleSpeed        = 0.0f;
   int      startFlyingTime   = 0;
   float    g                 = 0.0f;
   Vector2D v0                ( 0.0f,  0.0f);
   bool     mustBounce        = false;
   float    bounceCoef        = 0.0f;
   bool     mustBeCollided    = false;

   std::vector<float>      startColor(4, 1.0f);
   std::vector<float>      endColor(4, 1.0f);

   Vector2D boxSize  ( 1.0f, 1.0f);

   Vector2D cannonAlign (weaponDir.Perp());
   cannonAlign *= projCannonOffset;
   cannonAlign *= (weaponDir.x < 0) ? -1.0f : +1.0f;
   Vector2D pos = weaponPos + weaponDir * flameDist + cannonAlign;

   Particle* pPart = new Particle (
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprSize,
                                    sprSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize);

   Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_FLAMES);
   int mapIdx = RandInt(0,2);
   pPart->getSprite().setTexMapping(mapping[mapIdx]);

   pPart->getSprite().setCentered();
   pPart->getSprite().setColor(1.0f, 1.0f, 1.0f, RandInRange(0.2f, 0.5f));
   pPart->getSprite().setVisible();

   return pPart;
}
//
//Particle* Effects::ricochetSmoke(Game* particleMgr, Vector2D pos, OrientTBLR ori)
//{
//   int      lifeSpan = 500;
//
//   Vector2D sprStartSize;
//   float width = Tile::m_fWIDTH;
//   sprStartSize.x = width * 1.2f;
//   sprStartSize.y = width;
//
//   Vector2D sprEndSize (sprStartSize.x * 1.2f, sprStartSize.y * 1.2f );
//
//
//   float    curAngle          = 0.0f;
//   float    angleSpeed        = 0.0f;
//
//   switch (ori)
//   {
//      case TOP:
//         curAngle = HalfPi;
//         pos.y -= sprStartSize.x/2;
//         break;
//
//      case BOTTOM:
//         curAngle = -HalfPi;
//         pos.y += sprStartSize.x/2;
//         break;
//
//      case LEFT:
//         curAngle = Pi;
//         pos.x += sprStartSize.x/2;
//         break;
//
//      case RIGHT:
//         curAngle = 0.0f;
//         pos.x -= sprStartSize.x/2;
//         break;
//   }
//
//
//   int      startFlyingTime   = 0;
//   float    g                 = 0.0f;
//   Vector2D v0                ( 0.0f,  0.0f);
//   bool     mustBounce        = false;
//   float    bounceCoef        = 0.0f;
//   bool     mustBeCollided    = false;
//
//   float startRgb[] = {1.0f, 1.0f, 1.0f, 0.5f};
//   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
//   std::vector<float> startColor (startRgb, startRgb+4 );
//   std::vector<float> endColor (endRgb, endRgb+4 );
//
//   Vector2D boxSize;
//   boxSize.x = sprStartSize.x;
//   boxSize.y = sprStartSize.y;
//
//   Particle* pPart = new Particle (  particleMgr,
//                                    lifeSpan,
//                                    TEX_PARTICLES,
//                                    sprStartSize,
//                                    sprEndSize,
//                                    curAngle,
//                                    angleSpeed,
//                                    startFlyingTime,
//                                    g,
//                                    v0,
//                                    mustBounce,
//                                    bounceCoef,
//                                    mustBeCollided,
//                                    startColor,
//                                    endColor,
//                                    pos,
//                                    boxSize
//                                    );
//   Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_RICOCHET);
//   pPart->getSprite().setTexMapping(mapping[0]);
//   pPart->getSprite().setCentered();
//
//   return pPart;
//
//
//}
//
ParticleList Effects::BloodSplatter(Vector2D pos)
{
   int      lifeSpan = 500;
   float      size = Tile::m_fWIDTH * 0.20f;
   Vector2D sprStartSize(size, size);
   Vector2D sprEndSize(size*1.3f, size*1.3f);

   float    curAngle          = RandInRange(0, 2*Pi);
   float    angleSpeed        = HalfPi/8;
   int      startFlyingTime   = Time::ms();
   float    g                 = Tile::m_fWIDTH * 0.6f;
   Vector2D v0;
   bool     mustBounce        = false;
   float    bounceCoef        = 0.0f;
   bool     mustBeCollided    = false;


   float startRgb[] = {1.0f, 1.0f, 1.0f, 1.0f};
   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
   std::vector<float> startColor (startRgb, startRgb+4 );
   std::vector<float> endColor (endRgb, endRgb+4 );

   Vector2D boxSize( 1.0f, 1.0f );

   ParticleList partList;
   float offset = HalfPi/2;
   int numPart = RandInt(5, 10);

   for(int i=0; i < numPart; i++)
   {
      float angle = RandInRange(HalfPi-offset, HalfPi+offset);
      float length = Tile::m_fHEIGHT * RandInRange(0.05f, 0.15f);
      Vector2D v0 (angle);
      v0 *= length;

      Particle* pPart = new Particle(
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize
                                    );

      Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_BLOOD_SPLATTER);
      int mapId = RandInt(0,2);
      pPart->getSprite().setTexMapping(mapping[mapId]);
      pPart->getSprite().setCentered();

      partList.push_back(pPart);
   }

   return partList;
}

Particle* Effects::BloodCloud(Vector2D pos)
{
   int      lifeSpan = 1000;
   float      size = Tile::m_fWIDTH*0.3f;
   Vector2D sprStartSize (size, size);
   Vector2D sprEndSize ( size*4, size*4);

   float    curAngle          = RandInRange(0, 2*Pi);
   float    angleSpeed        = 0.0f;
   int      startFlyingTime   = Time::ms();
   float    g                 = Tile::m_fWIDTH * 0.04f;

   float    dirAngle = (RandInt(0,1)==0) ? RandInRange(Pi, 3*Pi/4) : RandInRange(Pi/4, 0);
   Vector2D v0 (dirAngle);
   float    length = Tile::m_fHEIGHT * RandInRange(0.01f, 0.02f);
   v0 *= length;

   bool     mustBounce        = false;
   float    bounceCoef        = 0.0f;
   bool     mustBeCollided    = false;

   float startRgb[] = {1.0f, 1.0f, 1.0f, 1.0f};
   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
   std::vector<float> startColor (startRgb, startRgb+4 );
   std::vector<float> endColor (endRgb, endRgb+4 );

   Vector2D boxSize(1.0f, 1.0f);

   Particle* pPart = new Particle (
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize
                                    );

   Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_BLOOD_CLOUD);
   int mapId = RandInt(0,1);
   pPart->getSprite().setTexMapping(mapping[mapId]);
   pPart->getSprite().setCentered();

   return pPart;
}


Particle* Effects::GibBigSplatter(const Vector2D& pos)
{
   int      lifeSpan = 2000;
   float      size = Character::m_fHEIGHT * 1.5f;
   Vector2D sprStartSize (size, size);
   Vector2D sprEndSize ( size*2, size*2);

   float    curAngle          = RandInRange(0, 2*Pi);
   float    angleSpeed        = 0.0f;
   int      startFlyingTime   = 0; // FIXME set this parameters into Particle class
   float    g                 = 0.0f;//Tile::m_fWIDTH * 0.02f;

   Vector2D v0;

   bool     mustBounce        = false;
   float    bounceCoef        = 0.0f;
   bool     mustBeCollided    = false;

   float startRgb[] = {1.0f, 1.0f, 1.0f, 1.0f};
   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
   std::vector<float> startColor (startRgb, startRgb+4 );
   std::vector<float> endColor (endRgb, endRgb+4 );

   Vector2D boxSize(1.0f, 1.0f);

   Particle* pPart = new Particle (
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    pos,
                                    boxSize
                                    );

   Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_GIB_SPLATTER);
   pPart->getSprite().setTexMapping(mapping[0]);
   pPart->getSprite().setCentered();

   return pPart;
}


Particle* Effects::GibLittleSplatter(const Vector2D& pos)
{
   Vector2D po (pos);
   int      lifeSpan = 1000;
   float    size = Tile::m_fWIDTH*0.3f;
   Vector2D sprStartSize (size, size);
   Vector2D sprEndSize ( size*4, size*4);

   float    curAngle          = RandInRange(0, 2*Pi);
   float    angleSpeed        = 0.0f;
   int      startFlyingTime   = Time::ms();
   float    g                 = Tile::m_fWIDTH * 0.02f;

   Vector2D v0;

   bool     mustBounce        = false;
   float    bounceCoef        = 0.0f;
   bool     mustBeCollided    = false;

   float startRgb[] = {1.0f, 1.0f, 1.0f, 1.0};
   float endRgb[]   = {1.0f, 1.0f, 1.0f, 0.0f};
   std::vector<float> startColor (startRgb, startRgb+4 );
   std::vector<float> endColor (endRgb, endRgb+4 );

   Vector2D boxSize(1.0f, 1.0f);



   Particle* pPart = new Particle (
                                    lifeSpan,
                                    TEX_PARTICLES,
                                    sprStartSize,
                                    sprEndSize,
                                    curAngle,
                                    angleSpeed,
                                    startFlyingTime,
                                    g,
                                    v0,
                                    mustBounce,
                                    bounceCoef,
                                    mustBeCollided,
                                    startColor,
                                    endColor,
                                    po,
                                    boxSize
                                    );

   Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_BLOOD_CLOUD);
   pPart->getSprite().setTexMapping(mapping[0]);
   pPart->getSprite().setCentered();

   return pPart;
}


ParticleList Effects::ThrowGiblets(const Vector2D& pos, ParticleManager& particleMgr)
{

   Vector2D sprSize;
   float    curAngle = 0.0f;
   float    angleSpeed = 0.0f;
   Vector2D v0;
   Float2DArray mappings = GFXAsset::Instance().getTexMapping(MAP_GIB);

   float w = Character::m_fWIDTH*4.5f;
   float h = Character::m_fHEIGHT*1.6f;

   float impCoefMin = 0.12f;
   float impCoefMax = impCoefMin * 2;

   float angleSpeedMin = HalfPi/16;
   float angleSpeedMax = HalfPi/8;

   float angleDirMin = Pi/4;
   float angleDirMax = 3*Pi/4;
   float angleDir = 0;

   ParticleList partList;

   // Head
   sprSize.x = w*0.25f;
   sprSize.y = h*0.15f;
   curAngle = 0.0f;
   angleSpeed = 0.0f;
   v0.x = cos(HalfPi);
   v0.y = sin(HalfPi);
   v0 *= RandInRange(impCoefMin, impCoefMax);
   Giblet* headGib = new Giblet(particleMgr, pos, sprSize, mappings[0], curAngle, angleSpeed, v0);
   partList.push_back(headGib);

   // Torso
   sprSize.x = w*0.2f;
   sprSize.y = h*0.2f;
   curAngle = 0.0f;
   angleSpeed = RandInRange(angleSpeedMin, angleSpeedMax);
   angleDir = RandInRange(angleDirMin, angleDirMax);
   v0.x = cos(angleDir);
   v0.y = sin(angleDir);
   v0 *= RandInRange(impCoefMin, impCoefMax);
   Giblet* torsoGib = new Giblet(particleMgr, pos, sprSize, mappings[1], curAngle, angleSpeed, v0);
   partList.push_back(torsoGib);

   // Arms
   sprSize.x = w*0.1f;
   sprSize.y = h*0.1f;
   curAngle = 0.0f;
   // Arm Left
   angleSpeed = RandInRange(angleSpeedMin, angleSpeedMax);
   angleDir = RandInRange(HalfPi, angleDirMax);
   v0.x = cos(angleDir);
   v0.y = sin(angleDir);
   v0 *= RandInRange(impCoefMin, impCoefMax);
   Giblet* armGibLeft = new Giblet(particleMgr, pos, sprSize, mappings[2], curAngle, angleSpeed, v0);
   partList.push_back(armGibLeft);
   // Arm Right
   angleSpeed = RandInRange(angleSpeedMin, angleSpeedMax);
   angleDir = RandInRange(angleDirMin, HalfPi);
   v0.x = cos(angleDir);
   v0.y = sin(angleDir);
   v0 *= RandInRange(impCoefMin, impCoefMax);;
   Giblet* armGibRight = new Giblet(particleMgr, pos, sprSize, mappings[2], curAngle, angleSpeed, v0);
   partList.push_back(armGibRight);

   // Forward Arms
   sprSize.x = w*0.1f;
   sprSize.y = h*0.1f;
   curAngle = 0.0f;
   // Forward Arm Left
   angleSpeed = RandInRange(angleSpeedMin, angleSpeedMax);
   angleDir = RandInRange(HalfPi, angleDirMax);
   v0.x = cos(angleDir);
   v0.y = sin(angleDir);
   v0 *= RandInRange(impCoefMin, impCoefMax);
   Giblet* forwardArmGibLeft = new Giblet(particleMgr, pos, sprSize, mappings[3], curAngle, angleSpeed, v0);
   partList.push_back(forwardArmGibLeft);
   // Forward Arm Right
   angleSpeed = RandInRange(angleSpeedMin, angleSpeedMax);
   angleDir = RandInRange(angleDirMin, HalfPi);
   v0.x = cos(angleDir);
   v0.y = sin(angleDir);
   v0 *= RandInRange(impCoefMin, impCoefMax);
   Giblet* forwardArmGibRight = new Giblet(particleMgr, pos, sprSize, mappings[3], curAngle, angleSpeed, v0);
   partList.push_back(forwardArmGibRight);

   // High Leg
   sprSize.x = w*0.1f;
   sprSize.y = h*0.1f;
   curAngle = 0.0f;
   // High Leg Left
   angleSpeed = RandInRange(angleSpeedMin, angleSpeedMax);
   angleDir = RandInRange(HalfPi, angleDirMax);
   v0.x = cos(angleDir);
   v0.y = sin(angleDir);
   v0 *= RandInRange(impCoefMin, impCoefMax);
   Giblet* highLegGibLeft = new Giblet(particleMgr, pos, sprSize, mappings[4], curAngle, angleSpeed, v0);
   partList.push_back(highLegGibLeft);
   // High Leg Right
   angleSpeed = RandInRange(angleSpeedMin, angleSpeedMax);
   angleDir = RandInRange(angleDirMin, HalfPi);
   v0.x = cos(angleDir);
   v0.y = sin(angleDir);
   v0 *= RandInRange(impCoefMin, impCoefMax);
   Giblet* highLegGibRight = new Giblet(particleMgr, pos, sprSize, mappings[4], curAngle, angleSpeed, v0);
   partList.push_back(highLegGibRight);

   //low leg
   sprSize.x = w*0.1f;
   sprSize.y = h*0.1f;
   curAngle = 0.0f;
   // Low Leg Left
   angleSpeed = RandInRange(angleSpeedMin, angleSpeedMax);
   angleDir = RandInRange(HalfPi, angleDirMax);
   v0.x = cos(angleDir);
   v0.y = sin(angleDir);
   v0 *= RandInRange(impCoefMin, impCoefMax);
   Giblet* lowLegGibLeft = new Giblet(particleMgr,  pos, sprSize, mappings[5], curAngle, angleSpeed, v0);
   partList.push_back(lowLegGibLeft);
   // Low Leg Right
   angleSpeed = RandInRange(angleSpeedMin, angleSpeedMax);
   angleDir = RandInRange(angleDirMin, HalfPi);
   v0.x = cos(angleDir);
   v0.y = sin(angleDir);
   v0 *= RandInRange(impCoefMin, impCoefMax);
   Giblet* lowLegGibRight = new Giblet(particleMgr,  pos, sprSize, mappings[5], curAngle, angleSpeed, v0);
   partList.push_back(lowLegGibRight);

   return partList;
}


//ParticleList Effects::worldDamaged(Game* particleMgr,
//                                           Vector2D& pos,
//                                           Vector2D& projDir)
//{
//   int      lifeSpan = 1200;
//   float    size = Tile::m_fWIDTH/10;
//   Vector2D sprStartSize(size, size);
//
//   float    curAngle          = 0.0f;
//   float    angleSpeed        = 0.0f;
//   int      startFlyingTime   = Time::ms();
//   float    g                 = Tile::m_fWIDTH * 0.4f;
//   Vector2D v0;
//   bool     mustBounce        = true;
//   float    bounceCoef        = 1.0f;
//   bool     mustBeCollided    = true;
//
//   float red   = RandInRange(0.65f, 0.85f);
//   float green = 0.34f;
//   float blue  = 0.24f;
//   float startRgb[] = {red, 0.34f, 0.24f, 1.0f};
//   float endRgb[]   = {red, 0.34f, 0.24f, 0.0f};
//   std::vector<float> startColor (startRgb, startRgb+4 );
//   std::vector<float> endColor (endRgb, endRgb+4 );
//
//   Vector2D boxSize( size, size);
//
//   ParticleList partList;
//
//   Vector2D z(1,0);
//   float angle = acos ( z.Dot(projDir) );
//   float angleOffset = HalfPi/2;
//
//   int numPart = RandInt(5, 10);
//   for(int i=0; i < numPart; i++)
//   {
//      float a = RandInRange(angle-angleOffset, angle+angleOffset);
//      Vector2D v0 ( std::cos(a),  std::sin(a) );
//
//      float length = Tile::m_fHEIGHT * RandInRange(0.05f, 0.15f);
//      v0 *= length;
//
//      Particle* pPart = new Particle( particleMgr,
//                                    lifeSpan,
//                                    TEX_PARTICLES,
//                                    sprStartSize,
//                                    sprStartSize,
//                                    curAngle,
//                                    angleSpeed,
//                                    startFlyingTime,
//                                    g,
//                                    v0,
//                                    mustBounce,
//                                    bounceCoef,
//                                    mustBeCollided,
//                                    startColor,
//                                    endColor,
//                                    pos,
//                                    boxSize
//                                    );
//
//      Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_WALL_FRAGMENT);
//      pPart->getSprite().setTexMapping(mapping[0]);
//      pPart->getSprite().setCentered();
//      partList.push_back(pPart);
//
//   }
//
//   return partList;
//}
