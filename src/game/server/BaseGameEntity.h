#ifndef BASEGAMEENTITY_H_INCLUDED
#define BASEGAMEENTITY_H_INCLUDED

/*!
* \file BaseGameEntity.h
* \brief Base class for an game entity
*/

#include <cassert>
#include <base/Vector2D.h>

/*!
*  \class BaseGameEntity
*  \brief This class is base for any game entity
*
*  Any entity inherites this class which contains base
*  properties for an entity
*/
class BaseGameEntity
{
   private:
      int m_iID;                    ///< each entity has a unique ID
      static int m_iNextValidID;    ///< this is next valid ID, updated for each entity created

      /*!
      *  \fn void SetID(int val)
      *  \brief this must be called within each constructor to make sure the ID
      *  is set correctly.
      *  \param val ID value of new entity
      */
      void SetID(int val);

   protected:
      Vector2D m_vPosition;      ///< Position of entity
      Vector2D m_vOldPosition;   ///< Old position of entity


   public:
   Vector2D m_vOldPos2;   ///< second Old position entity

      /*!
      *  \brief Constructor
      *  \param ID entity's ID
      */
      BaseGameEntity(int ID);
      /*!
      *  \brief Destructor
      */
      virtual ~BaseGameEntity();

      /*!
      *  \fn static int GetNextValidID()
      *  \brief Returns next valid ID
      *  \return int value which is next valid ID
      */
      static int GetNextValidID(){return m_iNextValidID;}

      /*!
      *  \fn int ID() const
      *  \brief Returns ID of entity
      *  \return int value which is ID of entity
      */
      int ID()const{return m_iID;}

      /*!
      *  \fn Vector2D Position()
      *  \brief Returns position of entity
      *  \return Vector2D which is position of entity
      */
      Vector2D Position()const {return m_vPosition;}

      //FIXME: should have an other name
      Vector2D* Pos() {return &m_vPosition;}

      /*!
      *  \fn void setPosition(Vector2D pos)
      *  \brief Sets position of entity
      *  \param pos new position of entity
      */
      void setPosition(Vector2D pos) {m_vPosition = pos;}

      /*!
      *  \fn void setPositionY(float y)
      *  \brief Sets y position of entity
      *  \param y new y position of entity
      */
      void setPositionY(float y) {m_vPosition.y = y;}

      /*!
      *  \fn void setPositionX(float x)
      *  \brief Sets x position of entity
      *  \param x new x position of entity
      */
      void setPositionX(float x) {m_vPosition.x = x;}

      /*!
      *  \fn Vector2D OldPosition()
      *  \brief Returns old position of entity
      *  \return Vector2D which is old position of entity
      */
      Vector2D OldPosition() {return m_vOldPosition;}

      bool isPosEqualsOldPos() const;
      void setOldPosEqualsToPos();

      void setOldPos(float x, float y);
      void setOldPos2(float x, float y);

      //Vector2D OldPosition2() const;
};

inline bool BaseGameEntity::isPosEqualsOldPos() const
{
   return m_vPosition == m_vOldPosition;
}

inline void BaseGameEntity::setOldPosEqualsToPos()
{
   m_vOldPosition.x = m_vPosition.x;
   m_vOldPosition.y = m_vPosition.y;
}

inline void BaseGameEntity::setOldPos(float x, float y)
{
   m_vOldPosition.x = x;
   m_vOldPosition.y = y;
}

inline void BaseGameEntity::setOldPos2(float x, float y)
{
   m_vOldPos2.x = x;
   m_vOldPos2.y = y;
}

#endif // BASEGAMEENTITY_H_INCLUDED
