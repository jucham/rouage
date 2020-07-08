#include "TriggerSystem.h"
#include "HealthGiver.h"
#include <base/debug.h>
#include <engine/shared/Snapshot.h>

TriggerSystem::TriggerSystem(Map &map) : m_Map(map)
{}

TriggerSystem::~TriggerSystem()
{
	std::list<Trigger*>::iterator curTrig = m_TriggerPtrs.begin();
	while (curTrig != m_TriggerPtrs.end())
	{
		delete *curTrig;
		curTrig = m_TriggerPtrs.erase(curTrig);
	}
}

void TriggerSystem::AddTrigger(Trigger* t)
{
	assert ( t != 0 );
	m_TriggerPtrs.push_back( t );
}

void TriggerSystem::UpdateTriggers()
{
	TriggerPtrsList::iterator curTrg = m_TriggerPtrs.begin();
	while (curTrg != m_TriggerPtrs.end())
	{
		//remove trigger if dead
		if ((*curTrg)->IsToBeRemoved())
		{
			delete *curTrg;
			curTrg = m_TriggerPtrs.erase(curTrg);
		}
		else
		{
			//update this trigger
			(*curTrg)->Update();
			++curTrg;
		}
	}
}

void TriggerSystem::GetSnapshot(NetTrigger *netTrigs, byte_t *skip)
{
	int i = 0;
	TriggerPtrsList::iterator it = m_TriggerPtrs.begin();
	while (it != m_TriggerPtrs.end())
	{
		if (!skip[i++]) {
			Trigger *pTrig = *it;
			netTrigs->exist = true;
			netTrigs->type = pTrig->m_Type;
			netTrigs->subType = pTrig->GetSubType(); // TODO packing possible
			netTrigs->active = pTrig->m_Active;
			netTrigs->x = pTrig->m_vPosition.x;
			netTrigs->y = pTrig->m_vPosition.y;
		}
		++netTrigs;
		++it;
	}
}
