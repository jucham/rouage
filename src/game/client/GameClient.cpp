#include <cmath>
#include "GameClient.h"
#include <engine/shared/AbsDim.h>
#include <engine/shared/NetPiece.h>
#include <engine/shared/Common.h>
#include <engine/client/Console.h>
#include <engine/client/sfx/SFXAsset.h>
#include <game/server/Character.h>
#include <base/tri_logger.hpp>
#include <engine/client/InputManager.h>
#include <game/client/ui/Monitor.h>
#include <base/Stats.h>
#include <engine/client/Client.h>
#include <engine/shared/MonitoringVars.h>
#include <game/server/Map.h>
#include <game/server/CollisionModel.h>
#include <game/ParticleCollisionModels.h>
#include "Effects.h"
#include "MapClient.h"
#include "ui/Menu.h"

int EventData::numEvents = 0;

GameClient::GameClient() :
				m_LocalClientId(INVALID_CLIENT_ID),
				m_Window(),
				m_Renderer(*this),
				m_frameRegulator(60, HERTZ_VALUE),
				m_CharactersData(MAX_CLIENTS),
				m_Monitoring(false),
				m_Chat(),
				m_pMap(0),
				m_ParticleMgr(m_Renderer.GetCam()),
				m_pColModPartTile(0),
				m_ActiveProjectileIds(),
				m_MenuStack(),
				m_MainMenu(MenuHandler::Instance().LoadAndSetInGameMenus(this)),
				m_GameLeft(false)

{
	AbsDim::initDimensions();

	for(int i=0; i<MAX_CLIENTS; i++) {
		m_CharactersData[i].exist = false;
		m_CharactersData[i].state = 0;
		m_CharactersData[i].pos = Vector2D();
		m_CharactersData[i].currentWeapon = 0;
		m_CharactersData[i].weaponAngle = 0;
		for (int j = 0; j < NUM_WEAPONS; j++) {
			m_CharactersData[i].aAmmoCount[j] = 0;
		}
		m_CharactersData[i].moveState = 0;
		m_CharactersData[i].health = 0;
		m_CharactersData[i].kills = 0;
		m_CharactersData[i].deaths = 0;
		std::sprintf(m_CharactersData[i].name, "Unamed Player");
	}

	for(int i=0; i<MAX_PROJECTILES; i++) {
		m_aProjectilesData[i].exist = false;
		m_aProjectilesData[i].weaponType = 0;
		m_aProjectilesData[i].pos = Vector2D();
		m_aProjectilesData[i].angle = 0;
		m_aProjectilesData[i].lastCloudTime = 0;
	}

	for(int i=0; i<MAX_TRIGGERS; i++) {
		m_aTriggerData[i].exist = false;
		m_aTriggerData[i].active = false;
		m_aTriggerData[i].pos = Vector2D();
		m_aTriggerData[i].type = 0;
		m_aTriggerData[i].subType = 0;
	}

	for(int i=0; i<MAX_EVENTS; i++) {
		m_aEventData[i].ownerClientId = 0;
		m_aEventData[i].pos = Vector2D();
		m_aEventData[i].type = 0;
	}

	TextRender::Instance().LoadFonts(Window::FrameHeight());
	Vector2D monitorOffset(10, Window::FrameHeight()-20);
	Monitor::Instance().Init(monitorOffset);
	SFXAsset::Instance();

	//TODO remove dependency to GameServer in Map as we don't need it there
	m_pMap = new Map(0);
	m_pColModPartTile = new CollisionModelParticleTile( m_ParticleMgr.Particles(), m_pMap->getTiles());

}

GameClient::~GameClient() {
	delete m_pMap;
	delete m_pColModPartTile;
}

void GameClient::SetLocalClientId(int clientId)
{
	m_LocalClientId = clientId;
}

CharacterData & GameClient::GetLocalCharacterData()
{
	return m_CharactersData[m_LocalClientId];
}

void GameClient::Update() {
	//	m_Chat.Update();
	//	m_KillMessages.Update();
	//	m_Score.Update();

	AddEffects();
	m_ParticleMgr.updateParticles();
	m_pColModPartTile->CollideEntities();

	UpdateConsole();
	UpdateChat();

	if (InputManager::Instance().IsKeyHit(SDL_SCANCODE_ESCAPE)) {
		if (m_MenuStack.Empty())
			m_MenuStack.PushMenu(m_MainMenu);
		else
			m_MenuStack.PopMenu();
	}

}

void GameClient::AddEffects() {

	int curTime = Time::ms();

	// --- CHARACTERS ---

	for (int clientId = 0; clientId < (int)m_CharactersData.size(); ++clientId)
	{
		CharacterData &c = m_CharactersData[clientId];

		// don't render inactive character
		if (c.state == INACTIVE)
			continue;

		if (c.moveState == MOVE_FLY) {

			Vector2D pos(c.pos.x + AbsDim::CHAR_W/2, c.pos.y + AbsDim::CHAR_H/3);
			Vector2D offset(AbsDim::JETPACK_W * 0.5f, 0.0f);

			if (cosf(c.weaponAngle) > 0.0f) {
				pos -= offset;
			} else {
				pos += offset;
			}

			Particle* pPart = Effects::JetpackSmokeCloud(pos);
			m_ParticleMgr.addParticle(pPart);
		}
	}

	// --- PROJECTILES ---

	for (int i = 0; i < MAX_PROJECTILES; ++i) {

		if (!m_aProjectilesData[i].exist)
			continue;

		int weaponType = m_aProjectilesData[i].weaponType;
		Vector2D projPosInPixel = Renderer::MeterToPixel(m_aProjectilesData[i].pos);

		switch (weaponType) {

		case WP_ROCKET_LAUNCHER:
		{
			if (curTime - m_aProjectilesData[i].lastCloudTime >= ProjectileData::CLOUD_DELAY)
			{
				float velFactor = 1.25f;
				Vector2D offset( std::cos(m_aProjectilesData[i].angle), std::sin(m_aProjectilesData[i].angle) );
				offset = offset.GetReverse();
				offset *= velFactor;

				Vector2D cloudPos (m_aProjectilesData[i].pos);
				cloudPos += offset;

				Particle* pPart = Effects::RocketSmokeCloud(cloudPos);
				m_ParticleMgr.addParticle(pPart);

				m_aProjectilesData[i].lastCloudTime = curTime;
			}
			break;
		}

		default:
			break;
		}
	}

	// --- EVENTS ---

	for (int i = 0; i < EventData::numEvents; i++)
	{
		Vector2D eventPosInPixel(Renderer::MeterToPixel(m_aEventData[i].pos));

		switch(m_aEventData[i].type) {

		case EVENT_ROCKET_EXPLODE:
		{
			ParticleList pParts = Effects::RocketExplosion(m_aEventData[i].pos);
			m_ParticleMgr.addParticles(pParts);
			break;
		}

		case EVENT_FIRE_PISTOL:
		{
			Vector2D weaponPos = m_CharactersData[m_aEventData[i].ownerClientId].pos;
			weaponPos.x += AbsDim::CHAR_W * 0.5f;
			weaponPos.y += AbsDim::CHAR_H * 0.75f;

			float weaponAngle = m_CharactersData[m_aEventData[i].ownerClientId].weaponAngle;
			Vector2D weaponDir(weaponAngle);

			Particle *pPart = Effects::ShotSmoke(weaponPos, weaponDir);
			m_ParticleMgr.addParticle(pPart);

			break;
		}

		case EVENT_FIRE_SHOTGUN:
		{
			Vector2D weaponPos = m_CharactersData[m_aEventData[i].ownerClientId].pos;
			weaponPos.x += AbsDim::CHAR_W * 0.5f;
			weaponPos.y += AbsDim::CHAR_H * 0.75f;

			float weaponAngle = m_CharactersData[m_aEventData[i].ownerClientId].weaponAngle;
			Vector2D weaponDir(weaponAngle);

			const Renderer::WeaponRender &weaponRender = m_Renderer.GetWeaponRender(WP_SHOTGUN);

			Particle *pPart = Effects::ShotSmoke(weaponPos, weaponDir);
			m_ParticleMgr.addParticle(pPart);

			pPart = Effects::ThrowCartridge(WP_SHOTGUN, weaponPos, weaponDir, weaponAngle, weaponRender.m_EjectionOffset);
			m_ParticleMgr.addParticle(pPart);

			break;
		}

		case EVENT_FIRE_AK47:
		{
			Vector2D weaponPos = m_CharactersData[m_aEventData[i].ownerClientId].pos;
			weaponPos.x += AbsDim::CHAR_W * 0.5f;
			weaponPos.y += AbsDim::CHAR_H * 0.75f;

			float weaponAngle = m_CharactersData[m_aEventData[i].ownerClientId].weaponAngle;
			Vector2D weaponDir(weaponAngle);
			const Renderer::WeaponRender &weaponRender = m_Renderer.GetWeaponRender(WP_AK47);

			Particle *pPart = 0;

			if (RandInt(0,2) == 0)
			{
				pPart = Effects::ShotFlame(weaponPos, weaponDir, weaponAngle, weaponRender.m_ProjCannonOffset, weaponRender.m_FlameDist);
				m_ParticleMgr.addParticle(pPart);
			}

			pPart = Effects::ShotSmoke(weaponPos, weaponDir);
			m_ParticleMgr.addParticle(pPart);

			pPart = Effects::ThrowCartridge(WP_AK47, weaponPos, weaponDir, weaponAngle, weaponRender.m_EjectionOffset);
			m_ParticleMgr.addParticle(pPart);

			break;
		}

		case EVENT_PAIN:
		{
			Vector2D projPos(m_aEventData[i].pos);

			ParticleList pParts = Effects::BloodSplatter(projPos);
			Particle* pPart = Effects::BloodCloud(projPos); //TODO proportional blood splatter to damage

			m_ParticleMgr.addParticles( pParts );
			m_ParticleMgr.addParticle( pPart );

			break;
		}

		case EVENT_DEATH:
		{
			break;
		}

		case EVENT_GIB_DEATH:
		{
			Vector2D pos(m_CharactersData[m_aEventData[i].ownerClientId].pos);
			pos += AbsDim::CHAR_H;
			ParticleList pParts = Effects::ThrowGiblets(pos, m_ParticleMgr);
			Particle* pPart = Effects::GibBigSplatter(pos);
			m_ParticleMgr.addParticles( pParts );
			m_ParticleMgr.addParticle( pPart );
			break;
		}

		case EVENT_DESTROY_TILE:
		{
			//TODO make reliable to be sure client rendering is consistent
			//TODO make effect
			m_Renderer.GetMapRender().DisableTile(m_aEventData[i].pos);
			break;
		}

		default:
			break;

		}
	}
}

void GameClient::RenderGame()
{
	CharacterData &localCharData = GetLocalCharacterData();
	Vector2D localCharPos(localCharData.pos.x, localCharData.pos.y);

	m_Renderer.ClearBuffer();
	m_Renderer.SetCamera(localCharPos);
	m_Renderer.InitRender();
	m_Renderer.SetLocalCharacterPos(localCharPos); // needed for sound rendering
	m_Renderer.RenderEventSounds(m_aEventData, m_ParticleMgr);
	EventData::numEvents = 0;
	m_Renderer.RenderMapBackground();
	m_Renderer.RenderProjectiles(m_aProjectilesData); //TODO render only in FOV
	m_Renderer.RenderMap();
	m_Renderer.RenderTriggers(m_aTriggerData);

	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (m_CharactersData[i].state != INACTIVE)
			m_Renderer.RenderCharacter(i, m_CharactersData[i], (i == m_LocalClientId));
	}

	m_ParticleMgr.RenderParticles();
	//	m_pCrosshair->render();
	m_Renderer.RenderChat(m_Chat.GetMessages());
	m_Renderer.RenderHUD(GetLocalCharacterData());

	if (InputManager::Instance().IsKeyPressed(SDL_SCANCODE_TAB))
			m_Renderer.RenderScoreBoard(m_CharactersData, m_LocalClientId);

	//   float x = m_cam.Position().x + 50;
	//   float y = m_cam.Position().y + 50;
	//   m_pHUD->render();
	//   m_pKillMsgs->update();
	//   m_pKillMsgs->render();

	//--------------------------------------------------------------------------
	// MONITOR
	//--------------------------------------------------------------------------

	static int swapBufferTimeAvg = 0;
	static StatCalc sc(10);
	Monitor::Instance().AddLine("Swap Buffer Time : %d ms", swapBufferTimeAvg);

	if (m_Monitoring)
	{
		Vector2D camPos = m_Renderer.GetCam().GetPos();

		// track position
		CharacterData &cd = GetLocalCharacterData();
		Vector2D pos(cd.pos);
		m_Renderer.MeterToPixel(pos);
		Monitor::Instance().AddLine("x = %.1f", pos.x);
		Monitor::Instance().AddLine("y = %.1f", pos.y);

		// snap stats
		float snapPercents = static_cast<float>(MonitoringVars::snapSize)/MAX_PACKET_SIZE;
		const int MAX_SNAP_BAR_VALUE = 40;
		char snapBar[MAX_SNAP_BAR_VALUE+1];
		BuildSnapBar(snapPercents, snapBar, MAX_SNAP_BAR_VALUE);
		Monitor::Instance().AddLine("Snapshot size (percents) : [%s]", snapBar);
		Monitor::Instance().AddLine("Snapshot Size Cur/Min/Max : %d/%d/%d on %d bytes (%.1f%%/%.1f%%/%.1f%%)",
				MonitoringVars::snapSize,
				MonitoringVars::snapSizeMin,
				MonitoringVars::snapSizeMax,
				MAX_PACKET_SIZE,
				(float)MonitoringVars::snapSize/MAX_PACKET_SIZE*100.0f,
				(float)MonitoringVars::snapSizeMin/MAX_PACKET_SIZE*100.0f,
				(float)MonitoringVars::snapSizeMax/MAX_PACKET_SIZE*100.0f);

		if (isEqual(0, MonitoringVars::snapCompRatio)) {
			Monitor::Instance().AddLine("Huffman Compression is off");
		} else {
			Monitor::Instance().AddLine("Huffman Compression Ratio : %0.2lf", MonitoringVars::snapCompRatio);
		}

		Monitor::Instance().Render(camPos);
	}
	Monitor::Instance().Reset();

	//--------------------------------------------------------------------------
	// USER INTERFACES
	//--------------------------------------------------------------------------
	m_Renderer.RenderUI();

	// --- SWAP BUFFER ---

	int start = Time::ms();
	m_Renderer.SwapBuffer();
	sc.AddValue(Time::ms() - start);
	if (sc.AllSampleCollected())
		swapBufferTimeAvg = sc.GetAverage();
}

void GameClient::BuildSnapBar(float snapPercents, char *snapBar, const int MAX_SNAP_BAR_VALUE) {

	int snapBarValue = std::floor(snapPercents*MAX_SNAP_BAR_VALUE + 0.5f);

	for (int i = 0; i < MAX_SNAP_BAR_VALUE; ++i) {
		snapBar[i] = ((i <= snapBarValue)?'#':'~');
	}
	snapBar[MAX_SNAP_BAR_VALUE] = '\0';

	const int PERCENT_SIZE = 5;
	char snapPercentStr[PERCENT_SIZE+1];
	int snapPercentSizeToCopy = 0;
	if(snapPercents < 0.1f) {
		snapPercentSizeToCopy = PERCENT_SIZE-2;
		std::sprintf(snapPercentStr,"%0.1f", snapPercents*100);
	} else if (snapPercents < 1.0f) {
		snapPercentSizeToCopy = PERCENT_SIZE-1;
		std::sprintf(snapPercentStr,"%0.1f", snapPercents*100);
	} else {
		snapPercentSizeToCopy = PERCENT_SIZE-2;
		std::sprintf(snapPercentStr,"%0.1f", snapPercents*100);
	}

	int offset = MAX_SNAP_BAR_VALUE/2-2;
	for (int i=0; i<snapPercentSizeToCopy; ++i) {
		snapBar[i+offset] = snapPercentStr[i];
	}
}

void GameClient::RenderStartscreen()
{
	m_Renderer.BeginMenu();

	// Show text with server connecting
	char text[100];
	char serverAddressStr[25];

	sprintf(serverAddressStr, "server");
	//	m_ServerAddress.ToString(serverAddressStr);

	// set waiting dots
	static char textDots[] = {'\0','.','.','.'};
	static int renderedFrames = 0;
	static int n = 0;
	++renderedFrames;
	if (renderedFrames >= 10) {
		renderedFrames = 0;
		char tmp = textDots[n];
		textDots[n] = textDots[(n+1)%4];
		textDots[(n+1)%4] = tmp;
		++n;
		n=n%4;
	}

	sprintf(text, "Connecting to %s", serverAddressStr);
	const int fontSize = 6;
	float textWidth = static_cast<float> (TextRender::Instance().GetFont(fontSize).calcStringWidth(text));
	sprintf(text, "Connecting to %s%s", serverAddressStr, textDots);
	TextRender::Instance().Render( -textWidth/2.0f, 0.0f, text, fontSize);

	m_Renderer.EndMenu();
}


void GameClient::RenderScreenMessage(const char *msg)
{
	m_Renderer.BeginMenu();
	const int fontSize = 6;
	float textWidth = static_cast<float> (TextRender::Instance().GetFont(fontSize).calcStringWidth(msg));
	TextRender::Instance().Render( -textWidth/2, 0, msg, fontSize);
	m_Renderer.EndMenu();
}

void GameClient::UpdateConsole() {

	if (InputManager::Instance().IsKeyHit(SDL_SCANCODE_F1)) {
		if (Console::Instance().IsActive()) {
			Console::Instance().SetActive(false);
			m_Renderer.DeactivateConsole();
		} else {
			Console::Instance().SetActive(true);
			m_Renderer.ActivateConsole();
		}
	}

	if (Console::Instance().IsActive()) {
		m_Renderer.UpdateConsole();
	}
}

void GameClient::UpdateChat() {

	if (!Console::Instance().IsActive())
	{
		if (InputManager::Instance().IsKeyHit(SDL_SCANCODE_K))
			m_Renderer.ToggleChat();

		if (InputManager::Instance().IsKeyHit(SDL_SCANCODE_RETURN)) {
			if (m_Renderer.FocusOnChat())
				m_Renderer.SetFocusOnChat(false);
			else
				m_Renderer.SetFocusOnChat(true);
		}
	}
}

bool GameClient::LoadMap(const char *pMapName)
{
	return m_Renderer.LoadMap(pMapName);
}

float GameClient::GetLocalWeaponAngle() {
	CharacterData &localCharData = GetLocalCharacterData();
	return m_Renderer.GetLocalWeaponAngle(localCharData);
}

void GameClient::UpdateClientData(int clientId)
{
	m_Renderer.UpdateSkin(clientId, m_CharactersData[clientId].skinId);
}


void GameClient::RenderMenu() {
	m_Renderer.BeginMenu();
	//	RenderScreenMessage("MENU");
	UpdateConsole();
	m_Renderer.EndMenu();
}

void GameClient::SetMonitoring(bool b) {
	m_Monitoring = b;
}

void GameClient::AddChatMessage(const char *msg) {
	m_Renderer.AddChatMessage(msg);
}


bool GameClient::ResumeGame(const CEGUI::EventArgs& ea) {
	m_MenuStack.PopMenu();
	return true;
}

bool GameClient::LeaveGame(const CEGUI::EventArgs& ea) {
	m_GameLeft = true;
	m_MenuStack.PopMenu();
	m_Renderer.ActiveMenuWindow();
	return true;
}

void GameClient::UnloadMap() {
	m_Renderer.UnloadMap();
}

