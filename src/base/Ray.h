#ifndef RAY_H
#define RAY_H

#include "Vector2D.h"


class Ray
{

   public:
      Ray(Vector2D origin,
         Vector2D direction,
         int curCellX,
         int curCellY,
         float cellWidth,
         float cellHeight,
         Vector2D gridOrigin);

      void step();

      int CurCellX() const;
      int CurCellY() const;
      Vector2D curBoundPosition() const;

   protected:
      Vector2D    m_vOrigin;
      Vector2D    m_vDirection;
      int 		   m_iCurCellX;
      int 		   m_iCurCellY;
      int 		   m_iStepX;
      int 		   m_iStepY;
      Vector2D 	m_vTmax; // distance that we can travel along the ray before hitting a cell boundary.
      Vector2D 	m_vTdelta; // distance that we can travel along the ray in crossing an only cell

};

inline int Ray::CurCellX() const {return m_iCurCellX;}
inline int Ray::CurCellY() const {return m_iCurCellY;}




#endif // RAY_H
