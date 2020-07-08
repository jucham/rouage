#ifndef TRIGGER_H_
#define TRIGGER_H_

#include "BaseGameEntity.h"
#include <base/AABBox2D.h>

struct Vector2D;
class Character;
class TriggerSystem;

class Trigger: public BaseGameEntity
{
   public:
	  friend class TriggerSystem;

      static float m_fWIDTH;
      static float m_fHEIGHT;

      Trigger(TriggerSystem* pTrigSys);
      virtual ~Trigger();

      void SetInactive()
      {
         m_Active = false;
      }
      void SetActive()
      {
         m_Active = true;
      }

      void setTouched()
      {
         m_Touched = true;
      }
      void SetUntouched()
      {
         m_Touched = false;
      }
      bool isTouched() const
      {
         return m_Touched;
      }

      bool IsToBeRemoved() const
      {
         return m_RemoveFromGame;
      }
      bool isActive() const
      {
         return m_Active;
      }
      bool mustDoAction() const
      {
         return m_MustDoAction;
      }

      void setCharTriggered(Character* pChar)
      {
         m_pCharTriggered = pChar;
      }

      AABBox2D* BBox()
      {
         return &m_HitBox;
      }

      virtual void DoAction() = 0;
      virtual void Update() = 0;
      virtual int GetSubType() const = 0;


   protected:
      TriggerSystem* m_pTrigSys;
      AABBox2D m_HitBox;
      int  m_Type;
      bool m_RemoveFromGame;
      bool m_Active;
      bool m_Touched;
      bool m_MustDoAction;
      Character* m_pCharTriggered;
      int m_GraphNodeIndex;
};

#endif // TRIGGER_H
