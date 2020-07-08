#ifndef SPRITE_H
#define SPRITE_H

#include <vector>
//#include <GL/glew.h>
#include <base/Vector2D.h>

class GFXAsset;
class VBO;

class Sprite
{
   public:
      Sprite(float width, float height, Vector2D offset=Vector2D());
      ~Sprite();

      void setPosition(float x, float y);
      void setPosWithAngle(float x, float y, float a, Vector2D offset=Vector2D());
      void setTexMapping(std::vector<float> mapping);
      void setTexMapping(float tlU=0.0f, float tlV=0.0f, float brU=1.0f, float brV=1.0f);
      void setColor(float r, float g, float b, float a);
      void setVisible();
      void setOffset(const Vector2D& pos);

      void flipX();
      void unFlipX();
      void flipY();
      void unFlipY();
      float Width() const;
      float Height() const;
      void  setSize(Vector2D v);
      void  setCentered();

      void getFlatData(float* dst, unsigned int fromPos);

      static void renderSpriteSet(const float* va, const VBO& vbo, const std::vector<int>& texIdxs, int numVertRendered = 4);

   protected:
      bool                 m_bFlipX; // flip setting is : false = initial image, true = flipped image
      bool                 m_bFlipY;
      float                m_fWidth;
      float                m_fHeight;
      Vector2D             m_vOffset;
      float*               m_posData;
      float*               m_texCoordData;
      float*               m_colData;
};


inline void Sprite::setVisible()
{
   setColor(1.0f, 1.0f, 1.0f, 1.0f);
}

#endif // SPRITE_H
