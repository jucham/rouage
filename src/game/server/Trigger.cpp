#include "Trigger.h"

float Trigger::m_fWIDTH; //FIXME recup ca a partir de AbsDim
float Trigger::m_fHEIGHT;

Trigger::Trigger(TriggerSystem* pTrigSys) :
      BaseGameEntity(BaseGameEntity::GetNextValidID()),
      m_pTrigSys(pTrigSys),
      m_HitBox(Vector2D(0, 0), m_fWIDTH, m_fHEIGHT),
      m_RemoveFromGame(false),
      m_Active(true),
      m_Touched(false),
      m_MustDoAction(true)
{}

Trigger::~Trigger()
{}
