#include "VBOSprite.h"
#include "GFXAsset.h"
#include <base/utils.h>


VBOSprite::VBOSprite(int texIdx,
               GLenum posAccessMode,
               GLenum texAccessMode,
               GLenum colAccessMode,
               float width,
               float height,
               Vector2D offset) :

   m_iTexIdx(texIdx),
   m_posVBO(m_POS_DATA_SIZE, posAccessMode, VertexBufferObject::vertexPointer, 2),
   m_texVBO(m_TEX_DATA_SIZE, texAccessMode, VertexBufferObject::texCoordPointer, 2),
   m_colVBO(m_COL_DATA_SIZE, colAccessMode, VertexBufferObject::colorPointer, 4),
   m_bFlipX(false),
   m_bFlipY(false),
   m_fWidth(width),
   m_fHeight(height),
   m_vOffset(offset)
{}

void VBOSprite::SetPosition(float x, float y)
{
   float pos[] =
   {
      x + m_fWidth,   y,
      x + m_fWidth,   y + m_fHeight,
      x,              y + m_fHeight,
      x,              y
   };

   for (int i=0; i<8; ++i)
   {
      pos[i] += (i%2==0) ? m_vOffset.x : m_vOffset.y;
   }

   m_posVBO.setData(pos);
}

void VBOSprite::SetPosition(Vector2D v)
{
   SetPosition(v.x, v.y);
}

void VBOSprite::setPosWithAngle(float x, float y, float a, Vector2D offset)
{

   float pos[] =
   {
      x + m_fWidth,   y,
      x + m_fWidth,   y + m_fHeight,
      x,              y + m_fHeight,
      x,              y
   };

   // Rotate
   float cx = x + m_fWidth/2; // center of sprite
   float cy = y + m_fHeight/2;

   for (int i=0; i<4; ++i)
   {
	  int m = 2*i;
	  int n = 2*i+1;

      float lx = pos[m] - cx;     // local x of point
      float ly = pos[n] - cy;   // local y of point

      pos[m] =     lx * cosf(a)         - ly * sinf(a) + cx; // angle a rotation
      pos[n] =   lx * sinf(a)         + ly * cosf(a) + cy;

      pos[m] += m_vOffset.x;
      pos[n] += m_vOffset.y;

      pos[m] += offset.x;
      pos[n] += offset.y;
   }

   m_posVBO.setData(pos);
}


void VBOSprite::setTexMapping( std::vector<float> mapping)
{
   setTexMapping(mapping[0], mapping[1], mapping[2], mapping[3]);
}

void VBOSprite::setTexMapping( float tlU,
                           float tlV,
                           float brU,
                           float brV)
{
   float tex[] =
   {
      brU,   brV,
      brU,   tlV,
      tlU,   tlV,
      tlU,   brV
   };

   if( m_bFlipX )
   {
      tex[0] = tlU;
      tex[6] = brU;
      tex[2] = tlU;
      tex[4] = brU;
   }

    if( m_bFlipY )
   {
      tex[1] = tlV;
      tex[3] = brV;
      tex[5] = brV;
      tex[7] = tlV;
   }

   m_texVBO.setData(tex);
}

void VBOSprite::setColor(float r, float g, float b, float a)
{
   float col[] =
   {
      r,g,b,a,
      r,g,b,a,
      r,g,b,a,
      r,g,b,a
   };

   m_colVBO.setData(col);

}



void VBOSprite::DirectToLeft()
{
   m_bFlipX = true; //TODO rename
}

void VBOSprite::DirectToRight()
{
   m_bFlipX = false;
}

void VBOSprite::DirectToTop()
{
   m_bFlipY = true;
}

void VBOSprite::DirectToBottom()
{
   m_bFlipY = false;
}

void VBOSprite::Render(bool bindTex)
{
   m_posVBO.update();
   m_texVBO.update();
   m_colVBO.update();

   VertexBufferObject::UnbindBuffer();

   if (bindTex)
   {
      glEnable(GL_TEXTURE_2D);
      GFXAsset::Instance().enableTex( m_iTexIdx );
   }

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);

   glDrawArrays(GL_QUADS, 0, 4);

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   // don't forget that, else could slow rendering
	if (bindTex)
	{
		glDisable(GL_TEXTURE_2D);
	}
}

float VBOSprite::Width() const
{
   return m_fWidth;
}

float VBOSprite::Height() const
{
   return m_fHeight;
}

void VBOSprite::setSize(Vector2D v)
{
   m_fWidth = v.x;
   m_fHeight = v.y;
}

void VBOSprite::SetOffset(const Vector2D& offset)
{
   m_vOffset = offset;
}

void VBOSprite::setCentered()
{
   m_vOffset.x = -m_fWidth/2;
   m_vOffset.y = -m_fHeight/2;
}
