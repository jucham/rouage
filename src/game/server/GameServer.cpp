#include "GameServer.h"

#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <base/Types.h>
#include "Map.h"
#include "CollisionManager.h"
#include "Projectile.h"
#include "TriggerSystem.h"
#include "EntityEnumerations.h"
#include "Tile.h"
#include <base/utils.h>
#include <engine/server/Server.h>
#include "Player.h"
#include <engine/shared/AbsDim.h>
#include <base/tri_logger.hpp>
#include <engine/shared/NetDataArray.h>
#include "WeaponSystem.h"

int GameServer::Event::numEvents = 0;

GameServer::GameServer(const Server &server) :
		m_Server(server),
		m_Map(0),
		m_CharacterPtrs(MAX_CLIENTS, 0),
		m_ProjectilePtrs(),
		m_CollisionMgr(0),
		m_GameRegulator(100, HERTZ_VALUE)
{
	AbsDim::initDimensions();

	m_Map = new Map(this);

	// Create players
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		m_CharacterPtrs[i] = new Player(this, i);
		m_CharacterPtrs[i]->setClientId(i);
		m_CharacterPtrs[i]->Reset();
	}

	m_CollisionMgr = new CollisionManager(this);

	Projectile::ResetUsedIds();
}

GameServer::~GameServer()
{
	delete m_Map;

	for (int i = 0; i < (int)m_CharacterPtrs.size(); ++i)
	{
		delete m_CharacterPtrs[i];
	}
	m_CharacterPtrs.clear();

	std::list<Projectile*>::iterator pCurProj = m_ProjectilePtrs.begin();
	while (pCurProj != m_ProjectilePtrs.end())
	{
		delete *pCurProj;
		pCurProj = m_ProjectilePtrs.erase(pCurProj);
	}

	delete m_CollisionMgr;
}

void GameServer::Update()
{
	// Update characters
	for (int i = 0; i < (int)m_CharacterPtrs.size(); ++i)
	{
		switch (m_CharacterPtrs[i]->GetState())
		{
		case INACTIVE:
			break;

		case ALIVE:
			m_CharacterPtrs[i]->Update();
			break;

		case DEAD:
		{
			// update stats
			m_ClientStats[i].deaths++;
			int killerClientId = m_CharacterPtrs[i]->GetKillerClientId();
			m_ClientStats[killerClientId].kills++;
			m_CharacterPtrs[i]->Die();
			break;
		}

		case SPAWNING:
			if ( m_CharacterPtrs[i]->CanSpawn() )
			{
				m_CharacterPtrs[i]->Spawn();

				const char *name = m_Server.GetClientName(i);
				if (std::strlen(name) == 0) {
					printf("client %d spawns at (%0.2lf, %0.2lf)\n", i, m_CharacterPtrs[i]->Position().x, m_CharacterPtrs[i]->Position().y);
				} else {
					printf("%s spawns at (%0.2lf, %0.2lf)\n", name, m_CharacterPtrs[i]->Position().x, m_CharacterPtrs[i]->Position().y);
				}
			}
			break;

		default:
			assert(false && "Wrong character state");
			break;
		}
	}

	// Update projectiles
	std::list<Projectile*>::iterator pCurProj = m_ProjectilePtrs.begin();
	while (pCurProj != m_ProjectilePtrs.end())
	{
		Projectile* pProj = *pCurProj;
		if ( pProj->Exist() )
		{
			pProj->update();
			++pCurProj;
		}
		else
		{
			delete *pCurProj;
			pCurProj = m_ProjectilePtrs.erase(pCurProj);
		}
	}

	// Update triggers
	m_Map->TriggerSys().UpdateTriggers();
}

void GameServer::Collide()
{
	// collides characters against all entity types
	m_CollisionMgr->collideCharacters();

	//collides projectiles against all entity types
	m_CollisionMgr->collideProjectiles();
}

void GameServer::OnTick()
{
	Update();
	Collide();
}

void GameServer::AddProjectile(Character* pOwner, const Vector2D& facing, const int weaponType)
{
	assert(pOwner != 0);
	Projectile* pProj = 0;

	Event event;
	event.pos.x = pOwner->WeaponSys()->WeaponPos().x;
	event.pos.y = pOwner->WeaponSys()->WeaponPos().y;
	event.ownerClientId = pOwner->GetClientId();

	switch (weaponType)
	{
	case WP_PISTOL:
	{
		pProj = new Bullet(pOwner, facing, 1.0f, 20); //TODO damage should be chosen into weapon class
		event.type = EVENT_FIRE_PISTOL;
		AddEvent(event);
		break;
	}
	case WP_ROCKET_LAUNCHER:
	{
		pProj = new Rocket(pOwner, facing);
		event.type = EVENT_FIRE_ROCKET_LAUNCHER;
		AddEvent(event);
		break;
	}

	case WP_AK47:
	{
		pProj = new Bullet(pOwner, facing, 1.0f, 7);
		event.type = EVENT_FIRE_AK47;
		AddEvent(event);
		break;
	}

	case WP_SHOTGUN:
	{
		pProj = new Bullet(pOwner, facing, RandInRange(0.7f, 1.0f), 9);
		break;
	}

	default:
		assert(false && "Bad projectile type");
		break;
	}

	pProj->setWeaponType(weaponType);
	m_ProjectilePtrs.push_back(pProj);
}

void GameServer::InitNewClients(const bool *pConnected)
{
	for (int i = 0; i < (int)m_CharacterPtrs.size(); ++i)
	{
		if ( pConnected[i] && m_CharacterPtrs[i]->GetState() == INACTIVE )
		{
			m_CharacterPtrs[i]->Init();
			m_ClientStats[i].kills = 0;
			m_ClientStats[i].deaths = 0;

			printf("Character of client %d initialized\n", i);
		}
	}
}

void GameServer::DeactivateDisconnectedClients(const bool *pConnected)
{
	for (int i = 0; i < (int)m_CharacterPtrs.size(); ++i)
	{
		if ( !pConnected[i] && m_CharacterPtrs[i]->GetState() != INACTIVE )
		{
			m_CharacterPtrs[i]->Reset();
			printf("Character of client %d deactivated\n", i);
		}
	}
}

void GameServer::AddEvent(Event &event)
{
	assert(event.type >= 0 && event.type < NUM_EVENTS);
	assert(Event::numEvents <= MAX_EVENTS);
	m_Events[Event::numEvents] = event;
	Event::numEvents++;
}

//TODO do not send events that player can't sense
void GameServer::GetEvents(NetPiece &np) const
{
	np.AddByte(Event::numEvents);
	for (int i = 0; i < Event::numEvents; ++i) {
		byte_t values =  m_Events[i].ownerClientId << 4;
		values |= m_Events[i].type;
		np.AddByte(&values);
		np.AddFloat(m_Events[i].pos.x);
		np.AddFloat(m_Events[i].pos.y);
	}
}

bool GameServer::IsServerFull() const {
	for(int i=0; i<MAX_CLIENTS; ++i)
		if (m_CharacterPtrs[i]->GetState() == INACTIVE)
			return false;
	return true;
}
