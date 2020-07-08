#ifndef VBOSPRITE_H
#define VBOSPRITE_H

#include <vector>
#include <GL/glew.h>
#include <base/Vector2D.h>
#include "VertexBufferObject.h"
#include "GFXAsset.h"


class VBOSprite
{
   public:
      VBOSprite(int texIdx,
               GLenum posAccessMode,
               GLenum texAccessMode,
               GLenum colAccessMode,
               float width,
               float height,
               Vector2D offset=Vector2D(0,0));

      void SetTextureId(int id);
      void SetPosition(float x, float y);
      void SetPosition(Vector2D v);
      void setPosWithAngle(float x, float y, float a, Vector2D offset=Vector2D());
      void setTexMapping(std::vector<float> mapping);
      void setTexMapping(float tlU = 0.0f, float tlV = 0.0f, float brU = 1.0f, float brV = 1.0f);
      void setColor(float r, float g, float b, float a);
      void SetVisible();
      void SetOffset(const Vector2D& pos);
      void DirectToLeft();
      void DirectToRight();
      void DirectToTop();
      void DirectToBottom();
      void Render(bool bindTex=true);
      float Width() const;
      float Height() const;
      void setSize(Vector2D v);
      void setCentered();

   protected:
      static const int m_POS_DATA_SIZE = 8*sizeof(float); // 4 x 2D vectors (x,y)
      static const int m_TEX_DATA_SIZE = 8*sizeof(float); // 4 x 2D vectors (u,v)
      static const int m_COL_DATA_SIZE = 16*sizeof(float); // 4 x 4D vectors (r,g,b,a)

      int                  m_iTexIdx;
      VertexBufferObject m_posVBO;
      VertexBufferObject m_texVBO;
      VertexBufferObject m_colVBO;
      bool m_bFlipX; // horizontal flip, false = initial image, true flipped image
      bool m_bFlipY;
      float m_fWidth;
      float m_fHeight;
      Vector2D m_vOffset;
};

inline void VBOSprite::SetTextureId(int id)
{
	assert(id >= 0 && id < NUM_TEXTURES);
	m_iTexIdx = id;
}

inline void VBOSprite::SetVisible()
{
   setColor(1.0f, 1.0f, 1.0f, 1.0f);
}




#endif // VBOSPRITE_H
