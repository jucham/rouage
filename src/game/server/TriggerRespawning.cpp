#include "TriggerRespawning.h"
#include "TriggerSystem.h"
#include <game/server/GameServer.h>

TriggerRespawning::TriggerRespawning(TriggerSystem* pTrigSys, GameServer *pGameServer) : Trigger(pTrigSys),
m_pGameServer(pGameServer),
m_RespawnDelay(10000),
m_TimeNextRespawn(0)
{}

TriggerRespawning::~TriggerRespawning()
{}

bool TriggerRespawning::MustRespawn()
{
	return Time::ms() >= m_TimeNextRespawn;
}

void TriggerRespawning::Update()
{
	if( isTouched() )
	{
		m_TimeNextRespawn = Time::ms() + m_RespawnDelay;
		DoAction();
		SetUntouched(); // it will be touched again when new collision
	}
	else if (MustRespawn())
	{
		SetActive();
	}
}
