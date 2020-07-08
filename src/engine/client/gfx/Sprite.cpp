#include "Sprite.h"
#include "GFXAsset.h"
#include <base/utils.h>
#include "../gfx/VBO.h"


#include <base/debug.h>

Sprite::Sprite(float width, float height, Vector2D offset) :
   m_bFlipX(false),
   m_bFlipY(false),
   m_fWidth(width),
   m_fHeight(height),
   m_vOffset(offset),
   m_posData(new float[8]),
   m_texCoordData(new float[8]),
   m_colData(new float[16])
{}

Sprite::~Sprite()
{
   delete[] m_posData;
   delete[] m_texCoordData;
   delete[] m_colData;
}

void Sprite::setPosition(float x, float y)
{
   m_posData[0] = x + m_fWidth;
   m_posData[1] = y;
   m_posData[2] = x + m_fWidth;
   m_posData[3] = y + m_fHeight;
   m_posData[4] = x;
   m_posData[5] = y + m_fHeight;
   m_posData[6] = x;
   m_posData[7] = y;

   for (int i=0; i<8; ++i)
   {
      // apply offset
      m_posData[i] += (i%2==0) ? m_vOffset.x : m_vOffset.y;
   }
}


void Sprite::setPosWithAngle(float x, float y, float a, Vector2D offset)
{
   m_posData[0] = x + m_fWidth;
   m_posData[1] = y;
   m_posData[2] = x + m_fWidth;
   m_posData[3] = y + m_fHeight;
   m_posData[4] = x;
   m_posData[5] = y + m_fHeight;
   m_posData[6] = x;
   m_posData[7] = y;

   // Rotate
   float cx = x + m_fWidth/2;
   float cy = y + m_fHeight/2;

   for (int i=0; i<4; ++i)
   {
      float lx = m_posData[2*i] - cx;     // local x of a quad point
      float ly = m_posData[2*i+1] - cy;   // local y of a quad point

      // angle a rotation and apply all offsets (instant and constant)
      m_posData[2*i]    =   (lx * cosf(a) - ly * sinf(a) + cx) + offset.x + m_vOffset.x; // angle a rotation
      m_posData[2*i+1]  =   (lx * sinf(a) + ly * cosf(a) + cy) + offset.y + m_vOffset.y;
   }

}

void Sprite::setTexMapping(std::vector<float> mapping)
{
   setTexMapping(mapping[0], mapping[1], mapping[2], mapping[3]);
}

void Sprite::setTexMapping(float tlU, float tlV, float brU, float brV)
{
   m_texCoordData[0] = brU;
   m_texCoordData[1] = brV;
   m_texCoordData[2] = brU;
   m_texCoordData[3] = tlV;
   m_texCoordData[4] = tlU;
   m_texCoordData[5] = tlV;
   m_texCoordData[6] = tlU;
   m_texCoordData[7] = brV;

   if( m_bFlipX )
   {
      m_texCoordData[0] = tlU;
      m_texCoordData[6] = brU;
      m_texCoordData[2] = tlU;
      m_texCoordData[4] = brU;
   }

   if( m_bFlipY )
   {
      m_texCoordData[1] = tlV;
      m_texCoordData[3] = brV;
      m_texCoordData[5] = brV;
      m_texCoordData[7] = tlV;
   }
}

void Sprite::setColor(float r, float g, float b, float a)
{
   for(int i=0; i<4; i++)
   {
	  int k = 4*i;
      m_colData[k] = r;
      m_colData[k+1] = g;
      m_colData[k+2] = b;
      m_colData[k+3] = a;
   }
}

void Sprite::flipX()
{
   m_bFlipX = true;
}

void Sprite::unFlipX()
{
   m_bFlipX = false;
}

void Sprite::flipY()
{
   m_bFlipY = true;
}

void Sprite::unFlipY()
{
   m_bFlipY = false;
}

float Sprite::Width() const
{
   return m_fWidth;
}

float Sprite::Height() const
{
   return m_fHeight;
}

void Sprite::setSize(Vector2D v)
{
   m_fWidth = v.x;
   m_fHeight = v.y;

   setCentered();
}

void Sprite::setOffset(const Vector2D& pos)
{
   m_vOffset = pos;
}

void Sprite::setCentered()
{
   m_vOffset.x = -m_fWidth/2;
   m_vOffset.y = -m_fHeight/2;
}

void Sprite::getFlatData(float* dst, unsigned int fromPos)
{

   for(unsigned int i=0; i<4; i++) // parse 4 vertices
   {
      // copy into dst all vertex data in that interleaved format
      // x, y, u, v, r, g, b, a, x, y, u, v, ...

      int k = 8*i + fromPos; // first index of a vertex
      int m = 2*i; // first index of a 2D coord
      int n = 4*i; // first index of a 4D coord

      // (x,y)
      dst[k]    = m_posData[m];
      dst[k+1]  = m_posData[m+1];

      // (u,v)
      dst[k+2] = m_texCoordData[m];
      dst[k+3] = m_texCoordData[m+1];

      // (r,g,b,a)
      for(int j=0; j<4; j++)
      {
         dst[k+4+j] = m_colData[n+j];
      }
   }
}

void Sprite::renderSpriteSet(const float* va,
                             const VBO& vbo,
                             const std::vector<int>& texIdxs,
                             int numVertRendered )
{

   int jmp = 8*sizeof(float);
#define VBO 0
#if VB0
   vbo.bindBuffer();
   vbo.update(va);
   glVertexPointer   (2, GL_FLOAT, jmp, BUFFER_OFFSET_BYTE(0));
   glTexCoordPointer (2, GL_FLOAT, jmp, BUFFER_OFFSET_BYTE(2*sizeof(float)));
   glColorPointer    (4, GL_FLOAT, jmp, BUFFER_OFFSET_BYTE(4*sizeof(float)));
   VBO::bindZeroBuffer();
#else
   glVertexPointer   (2, GL_FLOAT, jmp, &va[0]);
   glTexCoordPointer (2, GL_FLOAT, jmp, &va[2]);
   glColorPointer    (4, GL_FLOAT, jmp, &va[4]);
#endif

   //  draw quads one by one with different metatexture each time
   if(texIdxs.size() > 1)  numVertRendered = 4;

   glEnable(GL_TEXTURE_2D);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);

   for(int i=0; i<texIdxs.size(); i++)
   {
      GFXAsset::Instance().enableTex( texIdxs[i] );
      glDrawArrays(GL_QUADS, 0, numVertRendered);
   }

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisable(GL_TEXTURE_2D);

}
