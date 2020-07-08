#ifndef GAMESERVER_H_
#define GAMESERVER_H_

#include <string>
#include <vector>
#include <list>
#include <base/Time.h>
#include <base/Regulator.h>
#include "Character.h"
#include "TriggerSystem.h"
#include <engine/shared/Common.h>
#include <base/tri_logger.hpp>
#include <engine/shared/Snapshot.h>

class Map;
class Character;
class Projectile;
class CollisionManager;
class Server;

class GameServer
{

	friend class ServerSnapshotManager;

public:
	struct Event {
		static int numEvents;
		int ownerClientId;
		int type;
		Vector2D pos;
	};

	struct ClientStats {
		int kills;
		int deaths;
	};

private:
	const Server            &m_Server;
	Map                     *m_Map;
	std::vector<Character*> m_CharacterPtrs;
	std::list<Projectile*>  m_ProjectilePtrs;
	Event                   m_Events[MAX_EVENTS];
	CollisionManager        *m_CollisionMgr;
	Regulator               m_GameRegulator;
	ClientStats             m_ClientStats[MAX_CLIENTS];

public:
	GameServer(const Server &server);
	~GameServer();
	void OnTick();
	void exitGameServer();
	void Update();
	void Collide();
	void AddProjectile(Character* pShooter, const Vector2D& facing, const int weaponType);
	void AddEvent(Event &event);
	void GetEvents(NetPiece &np) const;
	void ResetEvents(int clientId);

	Map* GetMap() const
	{
		return m_Map;
	}

	CollisionManager* collisionMgr()
	{
		return m_CollisionMgr;
	}

	std::vector<Character*> & Characters()
			{
		return m_CharacterPtrs;
			}

	std::list<Projectile*> & Projectiles()
			{
		return m_ProjectilePtrs;
			}

	// TODO kick that
	bool isLOSOkay(const Vector2D& rayOrigin, const Character* target) const;
	bool canRayTraceToCharacter(const Vector2D& rayOrigin, const Character* target) const;
	bool canRayTrace(const Vector2D& rayOrigin, const Vector2D& targPos) const;
	void rayTrace(const Vector2D& rayOrigin, const Vector2D& rayDir);

	void SetInputs( int clientId, const unsigned char *pInputs, float weaponAngle)
	{
		assert(clientId >= 0 && clientId < MAX_CLIENTS);

		Character *pCharacter = m_CharacterPtrs[clientId];
		pCharacter->SetInputs(pInputs);
		pCharacter->SetWeaponAngle(weaponAngle);
	}


	void InitNewClients(const bool *pConnected);
	void DeactivateDisconnectedClients(const bool *pConnected);

	inline const ClientStats * GetClientStats() const;
	bool IsServerFull() const;

};

const GameServer::ClientStats * GameServer::GetClientStats() const {return m_ClientStats;}


#endif /* GAMESERVER_H_ */
