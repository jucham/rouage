#ifndef GAMECLIENT_H_
#define GAMECLIENT_H_

#include <list>
#include <base/Regulator.h>
#include <engine/client/Window.h>
#include <engine/client/gfx/GFXAsset.h>
#include <engine/client/gfx/text/TextRender.h>
#include <engine/shared/Common.h>
#include <engine/shared/Address.h>
#include <game/server/Map.h>
#include "Camera.h"
#include "Renderer.h"
#include "Chat.h"
#include "ParticleManager.h"
#include <CEGUI/System.h>
#include "ui/Menu.h"

class NetPiece;
class Map;
class CollisionModel;
class Client;

struct CharacterData
{
	bool exist;
	int state;
	Vector2D pos;
	int currentWeapon;
	float weaponAngle;
	int aAmmoCount[NUM_WEAPONS];
	int moveState;
	int health;
	int kills;
	int deaths;
	char name[MAX_PLAYERNAME_LENGTH];
	int skinId;
};

struct ProjectileData
{
	bool exist;
	int weaponType;
	Vector2D pos;
	float angle;

	// client only
	static const int CLOUD_DELAY = 50;
	int lastCloudTime;
};


struct EventData
{
	static int numEvents;
	int ownerClientId;
	int type;
	Vector2D pos;
};

struct TriggerData
{
	bool exist;
	int type;
	int subType;
	bool active;
	Vector2D pos;
};


class GameClient
{
	friend class MenuHandler;

public:

	GameClient();
	~GameClient();
	void Update();
	void RenderGame();
	void RenderMenu();
	void RenderStartscreen();
	CharacterData & GetCharacterData(int clientId);
	ProjectileData & GetProjectileData(int i);
	EventData * GetEventData();
	bool LoadMap(const char *pMapName);
	void UnloadMap();
	void SetLocalClientId(int clientId);
	float GetLocalWeaponAngle();
	Chat & GetChat() {return m_Chat;}
	TriggerData * GetTriggerData() {return m_aTriggerData;}
	int GetLocalClientId() const {return m_LocalClientId;}
	void AddEffects();
	void AddActiveProjectileId(int id);
	void ResetActiveProjectileIds();
	void UpdateClientData(int clientId);
	void SetMonitoring(bool b);
	Renderer & GetRenderer() { return m_Renderer; }
	void AddChatMessage(const char *msg);
	bool ResumeGame(const CEGUI::EventArgs& ea);
	bool LeaveGame(const CEGUI::EventArgs& ea);
	bool GameLeft() const {return m_GameLeft;}
	void SetGameLeft(bool b) {m_GameLeft = b;}
	void RenderScreenMessage(const char *msg);

private:
	CharacterData & GetLocalCharacterData();
	void UpdateConsole();
	void JoinGame();
	static void BuildSnapBar(float snapPercents, char *snapBar, const int MAX_SNAP_BAR_VALUE);
	void UpdateChat();

	int 						m_LocalClientId;
	Window 						m_Window;
	Renderer 					m_Renderer;
	Regulator 					m_frameRegulator;
	std::vector<CharacterData> 	m_CharactersData;
	ProjectileData 				m_aProjectilesData[MAX_PROJECTILES];
	EventData 					m_aEventData[MAX_EVENTS];
	TriggerData 				m_aTriggerData[MAX_TRIGGERS];
	bool 						m_Monitoring;
	Chat 						m_Chat;
	Map 						*m_pMap;
	ParticleManager 			m_ParticleMgr;
	CollisionModel 				*m_pColModPartTile;
	std::list<int> 				m_ActiveProjectileIds;
	MenuStack                   m_MenuStack;
	Menu 						*m_MainMenu;
	bool                        m_GameLeft;

};

inline CharacterData & GameClient::GetCharacterData(int clientId)
{
	assert(clientId >= 0 && clientId < MAX_CLIENTS);
	return m_CharactersData[clientId];
}

inline ProjectileData & GameClient::GetProjectileData(int i)
{
	assert(i >= 0 && i < MAX_PROJECTILES);
	return m_aProjectilesData[i];
}

inline EventData * GameClient::GetEventData()
{
	return m_aEventData;
}



inline void GameClient::AddActiveProjectileId(int id) {
	m_ActiveProjectileIds.push_back(id);
}

inline void GameClient::ResetActiveProjectileIds() {
	m_ActiveProjectileIds.clear();
}



#endif /* GAMECLIENT_H_ */
