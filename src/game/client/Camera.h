#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <base/Vector2D.h>
#include <engine/client/Window.h>

class Camera
{
   private:
      Vector2D m_vPosition;
      Vector2D m_FrameDims; // frame dimensions in meter unit

   public:
      Camera(Vector2D frameDims) : m_vPosition(0,0), m_FrameDims(frameDims) {}
      Camera(float x, float y) : m_vPosition(x,y) {}
      ~Camera() {}
      void     setPositionX(float x) {m_vPosition.x = x;}
      void     setPositionY(float y) {m_vPosition.y = y;}
      void SetFrameDims(Vector2D v) {m_FrameDims = v;}
      Vector2D GetPos() const {return m_vPosition;}
      void updatePos(Vector2D v) {m_vPosition += v;}
      bool isIntoFOV(const Vector2D& pos) const;
};

inline bool Camera::isIntoFOV(const Vector2D& pos) const
{
   // let see objects when they are outside screen and half inside screen
   // object are rendered if they are near of screen limit.
   Vector2D offset(0.15f*m_FrameDims.x, 0.15f*m_FrameDims.y);

   return  pos.x >= m_vPosition.x - offset.x
            && pos.y >= m_vPosition.y - offset.y
            && pos.x <= m_vPosition.x + m_FrameDims.x + offset.x
            && pos.y <= m_vPosition.y + m_FrameDims.y + offset.y;
}

#endif // CAMERA_H_INCLUDED
