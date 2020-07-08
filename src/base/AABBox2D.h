#ifndef AABBOX2D_H_INCLUDED
#define AABBOX2D_H_INCLUDED

#include <cmath>
#include "Vector2D.h"
#include <base/debug.h>

class AABBox2D
{
   private:
      Vector2D m_vLeftBottom;
      Vector2D m_vRightTop;
      Vector2D m_vCenter;
      float m_fWidth;
      float m_fHeight;

   public:

      AABBox2D()
      {
         AABBox2D(Vector2D(0,0), 0.0, 0.0);
      }

      AABBox2D(Vector2D leftBottom, float width, float height)
      {
         m_vLeftBottom = leftBottom;
         m_fWidth = width;
         m_fHeight = height;
         m_vRightTop = leftBottom + Vector2D(width, height);
         m_vCenter = leftBottom + Vector2D(width/2, height/2);
      }

      float Top() const {return m_vRightTop.y;}
      float Bottom() const {return m_vLeftBottom.y;}
      float Left() const {return m_vLeftBottom.x;}
      float Right() const {return m_vRightTop.x;}

      float Width() const {return m_fWidth;}
      float Height() const {return m_fHeight;}

      const Vector2D& Position() const {return m_vLeftBottom;}
      const Vector2D& Center() const {return m_vCenter;}

      void setPosition(Vector2D position)
      {
         m_vLeftBottom = position;
         m_vRightTop = position + Vector2D(m_fWidth, m_fHeight);
         m_vCenter = position + Vector2D(m_fWidth/2, m_fHeight/2);
      }

      void setPositionX(float x)
      {
         m_vLeftBottom.x = x;
         m_vRightTop.x = x + m_fWidth;
      }

      void setPositionY(float y)
      {
         m_vLeftBottom.y = y;
         m_vRightTop.y = y + m_fHeight;
      }

      void setWidth(float width)
      {
         m_fWidth = width;
      }

      void setHeight(float height)
      {
          m_fHeight = height;
      }

      bool isOverlappedWith(AABBox2D bbox, float* xOverlap = 0, float* yOverlap = 0)
      {
		 // distance with the lengths of the two box confused
         float xConfusedDist = (Left()-bbox.Left()>0) ? std::abs(Right()-bbox.Left()) : std::abs( bbox.Right()-Left());
         float yConfusedDist = (Bottom()-bbox.Bottom()>0) ? std::abs(Top()-bbox.Bottom()) : std::abs(bbox.Top()-Bottom());

         if(xOverlap != NULL && yOverlap != NULL)
         {
            *xOverlap = m_fWidth+bbox.Width() - xConfusedDist;
            *yOverlap = m_fHeight+bbox.Height() - yConfusedDist;
         }

		 // if on x and on y the distance of boxes confused is less than than the sum of theirs length then it's overlapping
         return (xConfusedDist < m_fWidth + bbox.m_fWidth) && (yConfusedDist < m_fHeight + bbox.m_fHeight);       
      }

};

#endif // AABBOX2D_H_INCLUDED
