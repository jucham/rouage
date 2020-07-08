#include "Ray.h"

Ray::Ray(Vector2D origin,
         Vector2D direction,
         int curCellX,
         int curCellY,
         float cellWidth,
         float cellHeight,
         Vector2D gridOrigin) :

   m_vOrigin(origin),
	m_vDirection(direction),
	m_iCurCellX(curCellX),
	m_iCurCellY(curCellY),
	m_iStepX(0),
	m_iStepY(0),
    m_vTmax(0,0),
    m_vTdelta(0,0)

{

	Vector2D cellBound(0,0);

	if (direction.x > 0.0f)
	{
		m_iStepX = 1;
		cellBound.x = gridOrigin.x + (m_iCurCellX+1) * cellWidth;
	}
	else
	{
		m_iStepX = -1;
		cellBound.x = gridOrigin.x + m_iCurCellX * cellWidth;
	}

	if (direction.y > 0.0f)
	{
		m_iStepY = 1;
		cellBound.y = gridOrigin.y + (m_iCurCellY+1) * cellHeight;
	}
	else
	{
		m_iStepY = -1;
		cellBound.y = gridOrigin.y + m_iCurCellY * cellHeight;
	}

	// --- Determine  m_vTmax and m_vTdelta ---
	// To compute Tmax or Tdelta we use the same proportionality rule following
	// By example, to compute the first tmax on X
	// -----------------------------------------------------------------------------------------------------
	// Traveled distance on X		|	direction.x					 		|	(cellBound.x - origin.x)	|
	// ----------------------------------------------------------------------------------------------
	// Traveled distance by ray 	|	1 (direction is normalized)	|	??? (first m_vTmax)		|
	//------------------------------------------------------------------------------------------------------
	// That gives : ((cellBound.x - origin.x) * 1) / raydir.x = first Tmax
	// where (cellBound.x - origin.x) is traveled distance on X from ray origin
	// to the next bound that separates two cells
	//
	if (direction.x != 0.0f)
	{
		m_vTmax.x = (cellBound.x - origin.x) / direction.x; //  (cellBound.x - origin.x)
		m_vTdelta.x = (cellWidth * m_iStepX) / direction.x;
	}
	else m_vTmax.x = 1000000;

	if (direction.y != 0.0f)
	{
		m_vTmax.y = (cellBound.y - origin.y) / direction.y;
		m_vTdelta.y = (cellHeight * m_iStepY) / direction.y;
	}
	else m_vTmax.y = 1000000;

}

void Ray::step()
{
	if (m_vTmax.x < m_vTmax.y)
	{
	   m_vTmax.x += m_vTdelta.x;
	   m_iCurCellX += m_iStepX;
	}
	else
	{
	   m_vTmax.y += m_vTdelta.y;
	   m_iCurCellY += m_iStepY;
	}
}

Vector2D Ray::curBoundPosition() const
{
   Vector2D scaledRay;

   if (m_vTmax.x < m_vTmax.y)
   {
      scaledRay = m_vDirection * m_vTmax.x;
   }
   else
   {
      scaledRay = m_vDirection * m_vTmax.y;
   }

   return m_vOrigin + scaledRay;
}
