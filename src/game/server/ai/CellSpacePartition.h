#ifndef CELLSPACEPARTITION_H_INCLUDED
#define CELLSPACEPARTITION_H_INCLUDED

#include <vector>
#include <list>
#include <cassert>

#include <base/Vector2D.h>
#include <base/AABBox2D.h>
#include <base/utils.h>


///
/// \class Cell CellSpacePartition.h
/// \brief defines a cell containing an entity list
///
template <class entity>
class Cell
{
   public:

      /// \brief Constructs a cell
      Cell(Vector2D leftBottom, Vector2D rightTop);

      /// \brief Add an entity \a ent into the cell
      void addEntity(const entity& ent);

      /// \brief Renders cell if \a isFocused is true
      void render(bool isFocused) const;

      /// \brief Get all members containded into the cell
      const std::list<entity>& getMembers() const;

   protected:

      Vector2D          m_vLeftBottom;
      Vector2D          m_vRightTop;
      std::list<entity> m_members;  ///< all the entities inhabiting this cell
};

template<class entity>
Cell<entity>::Cell(Vector2D leftBottom, Vector2D rightTop) :
   m_vLeftBottom(leftBottom),
   m_vRightTop(rightTop),
   m_members()
{}


template<class entity>
void Cell<entity>::addEntity(const entity& ent)
{
   m_members.push_back(ent);
}


template<class entity>
const std::list<entity>& Cell<entity>::getMembers() const
{
   return m_members;
}


///
/// \class CellSpacePartition CellSpacePartition.h
/// \brief defines a cell space partition
///
template <class entity>
class CellSpacePartition
{
   private:

      std::vector<Cell<entity> >  m_Cells;          ///< the required amount of cells in the space
      float                       m_fSpaceWidth;    ///< sum of all cells on width
      float                       m_fSpaceHeight;   ///< sum of all cells on height
      int                         m_iNumCellsX;     ///< number of cells on X
      int                         m_iNumCellsY;     ///< number of cells on Y
      float                       m_fCellSizeX;     ///< size of one cell on X
      float                       m_fCellSizeY;     ///< size of one cell on Y

   public:

      /// \brief Constructs a cell space.
      ///
      /// Constructs a cell space with dimension \a width x \a height, with
      /// \a cellsX cells on X and \a cellsY cells on Y
      ///
      CellSpacePartition(float width,
                        float height,
                        int   cellsX,
                        int   cellsY);

      void createCells();
      void EmptyCells();

      /// \brief give the cell index that matches with a position in world
      int PositionToIndex(const Vector2D& pos)const;

      void RenderCells() const;
      void RenderCells(int focusIdx) const;
      void setNumCells(int numCellsX, int numCellsY);
      void setSpaceDim(float spaceWidth, float spaceHeight);
      void addEntity(const entity& ent);


      float                SpaceWidth() const;
      float                SpaceHeight() const;
      const Cell<entity>&  getCellByIndex(int idx) const;

};


template<class entity>
CellSpacePartition<entity>::CellSpacePartition(float  width,
                                               float  height,
                                               int    cellsX,
                                               int    cellsY) :
   m_Cells(),
   m_fSpaceWidth(width),
   m_fSpaceHeight(height),
   m_iNumCellsX(cellsX),
   m_iNumCellsY(cellsY),
   m_fCellSizeX(0.0f),
   m_fCellSizeY(0.0f)
{}


template<class entity>
void CellSpacePartition<entity>::createCells()
{
   //calculate dimensions of each cell
   m_fCellSizeX = m_fSpaceWidth / m_iNumCellsX;
   m_fCellSizeY = m_fSpaceHeight / m_iNumCellsY;

   //create the cells
   for (int y=0; y<m_iNumCellsY; ++y)
   {
      for (int x=0; x<m_iNumCellsX; ++x)
      {
         float left    = x * m_fCellSizeX;
         float bottom  = y * m_fCellSizeY;
         float right   = left + m_fCellSizeX;
         float top     = bottom + m_fCellSizeY;

         m_Cells.push_back( Cell<entity>(Vector2D(left, bottom), Vector2D(right, top)) );
      }
   }
}


template<class entity>
void CellSpacePartition<entity>::EmptyCells()
{
  typename std::vector<Cell<entity> >::iterator it = m_Cells.begin();

  for (it; it!=m_Cells.end(); ++it)
  {
    it->m_members.clear();
  }
}


template<class entity>
inline int CellSpacePartition<entity>::PositionToIndex(const Vector2D& pos)const
{
  int idx =    static_cast<int>(m_iNumCellsX * pos.x / m_fSpaceWidth)
            + static_cast<int>(m_iNumCellsY * pos.y / m_fSpaceHeight) * m_iNumCellsX;

  //if the entity's position is equal to vector2d(m_fSpaceWidth, m_fSpaceHeight)
  //then the index will overshoot. We need to check for this and adjust
  if (idx > m_Cells.size()-1) idx = m_Cells.size()-1;

  return idx;
}


template<class entity>
inline void CellSpacePartition<entity>::RenderCells()const
{
 typename std::vector<Cell<entity> >::const_iterator curCell;
  for (curCell=m_Cells.begin(); curCell!=m_Cells.end(); ++curCell)
  {
    curCell->render(false);
  }
}


template<class entity>
inline void CellSpacePartition<entity>::RenderCells(int focusIdx)const
{
   for(int i=0; i<m_Cells.size(); ++i)
   {
      m_Cells[i].render(false);
   }
   m_Cells[focusIdx].render(true);

}


template<class entity>
void CellSpacePartition<entity>::setNumCells( int numCellsX,
                                             int numCellsY)
{
   m_iNumCellsX = numCellsX;
   m_iNumCellsY = numCellsY;
}


template<class entity>
void CellSpacePartition<entity>::setSpaceDim(float spaceWidth,
                                             float spaceHeight)
{
   m_fSpaceWidth = spaceWidth;
   m_fSpaceHeight = spaceHeight;
}


template<class entity>
inline void CellSpacePartition<entity>::addEntity(const entity& ent)
{
  int idx = PositionToIndex(ent->Position());
  m_Cells[idx].addEntity(ent);
}


template<class entity>
float CellSpacePartition<entity>::SpaceWidth() const
{
   return m_fSpaceWidth;
}


template<class entity>
float CellSpacePartition<entity>::SpaceHeight() const
{
   return m_fSpaceHeight;
}

template<class entity>
const Cell<entity>& CellSpacePartition<entity>::getCellByIndex(int idx) const
{

   assert (idx >= 0 && idx <= m_Cells.size() );
   return m_Cells[idx];

}


#endif // CELLSPACEPARTITION_H_INCLUDED
