#include "BaseGameEntity.h"

int BaseGameEntity::m_iNextValidID = 0;


BaseGameEntity::BaseGameEntity(int ID) :
   m_vPosition(0.0f, 0.0f),
   m_vOldPosition(-1.0f, -1.0f),
   m_vOldPos2(-1.0f, -1.0f)
{
  #if DBG
            dbg << "## start  Base Game Entity init" << std::endl;
    #endif

  SetID(ID);

  #if DBG
            dbg << "## end  Base Game Entity init" << std::endl;
    #endif
}

BaseGameEntity::~BaseGameEntity() {}


void BaseGameEntity::SetID(int val)
{
  //make sure the val is equal to or greater than the next available ID
  assert ( (val >= m_iNextValidID) && "<BaseGameEntity::SetID>: invalid ID");

  m_iID = val;

  m_iNextValidID = m_iID + 1;
}
