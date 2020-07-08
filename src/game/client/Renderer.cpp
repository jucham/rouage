#include "Renderer.h"
#include <GL/glew.h>
#include <engine/shared/MapLoader.h>
#include <engine/client/gfx/GFXAsset.h>
#include <engine/client/gfx/VBOSprite.h>
#include <engine/client/sfx/SFXAsset.h>
#include <engine/client/InputManager.h>
#include "MapClient.h"
#include "Camera.h"
#include "GameClient.h"
#include <base/tri_logger.hpp>
#include <engine/client/Console.h>
#include <engine/client/ui/FormattedListboxTextItem.h>



float Renderer::m_PixelsPerMeter = 0.0f;

void Renderer::ResizeBackground() {
	float ratioDst = Window::m_RealFrameDims.x / Window::m_RealFrameDims.y;
	float ratioSrc = 1024.0f / 768;
	float ratioFactor = ratioDst / ratioSrc;
	Vector2D bgDims(Window::m_FrameWidth * ratioFactor, Window::m_FrameHeight);
	m_BackgroundSprite.setSize(bgDims);
	Vector2D offset((Window::m_FrameWidth - bgDims.x) / 2, 0);
	m_BackgroundSprite.SetOffset(offset);
}

Renderer::Renderer(GameClient &gameClient) :
				m_pMapRender(0),
				m_HitBoxSprite(TEX_WHITE, GL_STREAM_DRAW, GL_STREAM_DRAW, GL_STATIC_DRAW, 0, 0),
				m_TriggerRenderPtrs(MAX_TRIGGERS, 0),
				m_CharacterRenderPtrs(MAX_CLIENTS, 0),
				m_ProjectileSpritePtrs(MAX_PROJECTILES, 0),
				m_Cam(PixelToMeter(Vector2D(Window::FrameWidth(), Window::FrameHeight()))),
				m_GameClient(gameClient),
				m_LocalCharPos(),
				m_pHudRender(0),
				m_ConsoleRender(),
				m_StartscreenSprite(TEX_STARTSCREEN, GL_STATIC_DRAW, GL_STATIC_DRAW, GL_STATIC_DRAW, Window::FrameWidth(), Window::FrameHeight()),
				m_BackgroundSprite(TEX_BACKGROUND, GL_STATIC_DRAW, GL_STATIC_DRAW, GL_STATIC_DRAW, Window::m_RealFrameDims.x, Window::m_RealFrameDims.y),
				m_MenuBackgroundSprite(TEX_MENU_BACKGROUND, GL_STATIC_DRAW, GL_STATIC_DRAW, GL_STATIC_DRAW, Window::FrameWidth() * 0.6f, Window::FrameHeight() * 1.1f)
{
	InitTileDimensions();

	m_HitBoxSprite.setSize(Vector2D(m_HitBoxCharW, m_HitBoxCharH));
	m_HitBoxSprite.SetVisible();

	ResizeBackground();

	m_MenuBackgroundSprite.SetVisible();
	m_MenuBackgroundSprite.setTexMapping();
	m_MenuBackgroundSprite.setCentered();
	m_MenuBackgroundSprite.SetPosition(0, 0);

	CreateTriggerRenders();
	CreateCharacterRenders();
	CreateProjectileSprites();
	ProjectileSettings();
	WeaponSettings();

	SoundPlayer::Instance().SetListenerPos(&m_LocalCharPos);

	m_pHudRender = new HudRender(m_HitBoxTileSize);

	InitCEGUI();
}

void Renderer::CreateTriggerRenders() {
	for (int i = 0; i < MAX_TRIGGERS; i++) {
		m_TriggerRenderPtrs[i] = new TriggerRender();
	}
}

void Renderer::CreateCharacterRenders()
{
	Vector2D charDim(m_HitBoxCharW * 4.5f, m_HitBoxCharH * 1.6f);
	Vector2D armDim(m_CharArmW, m_CharArmH);
	Vector2D jetpackDim(m_JetpackW, m_JetpackH);
	Vector2D charOffset(-m_HitBoxCharW * 1.75f, -m_HitBoxCharH * 0.33f);
	for (int clientId = 0; clientId < MAX_CLIENTS; ++clientId)
	{
		CharacterRender *pCharRender = new CharacterRender(charDim, charOffset, armDim, jetpackDim, GFXAsset::Instance());
		pCharRender->m_BodySprite.SetVisible();
		pCharRender->m_ArmSprite.SetVisible();
		pCharRender->m_WeaponSprite.SetVisible();
		pCharRender->m_JetpackSprite.SetVisible();
		m_CharacterRenderPtrs[clientId] = pCharRender;
	}
}

void Renderer::CreateProjectileSprites()
{
	for (int i = 0; i < MAX_PROJECTILES; i++)
	{
		m_ProjectileSpritePtrs[i] = new VBOSprite(TEX_PROJECTILES, GL_STREAM_DRAW, GL_STREAM_DRAW, GL_STATIC_DRAW, 0, 0);
	}
}

void Renderer::WeaponSettings()
{
	// weapon sprite is centered to the weapon position then it's shifted according to the type of weapon
	for (int i = 0; i < NUM_WEAPONS; ++i) {
		m_WeaponRenders[i].m_HandOffset = m_WpDims[i].width * 0.5f;
	}
	m_WeaponRenders[WP_PISTOL].m_HandOffset += m_CharArmW * 0.5f;
	m_WeaponRenders[WP_ROCKET_LAUNCHER].m_HandOffset += m_CharArmW * 0.1f;
	m_WeaponRenders[WP_SHOTGUN].m_HandOffset += m_CharArmW * 0.1f;
	//	m_WeaponRenders[WP_AK47].m_HandOffset += m_CharArmW * 0.1f;

	m_WeaponRenders[WP_PISTOL].m_EjectionOffset = 1.0f;
	m_WeaponRenders[WP_ROCKET_LAUNCHER].m_EjectionOffset = 1.0f;
	m_WeaponRenders[WP_SHOTGUN].m_EjectionOffset = PixelToMeter(m_WeaponRenders[WP_SHOTGUN].m_HandOffset);
	m_WeaponRenders[WP_AK47].m_EjectionOffset = PixelToMeter(m_WeaponRenders[WP_AK47].m_HandOffset);

	m_WeaponRenders[WP_PISTOL].m_ProjCannonOffset = 1.0f;
	m_WeaponRenders[WP_ROCKET_LAUNCHER].m_ProjCannonOffset = 1.0f;
	m_WeaponRenders[WP_SHOTGUN].m_ProjCannonOffset = 1.0f;
	m_WeaponRenders[WP_AK47].m_ProjCannonOffset = PixelToMeter(m_WpDims[WP_AK47].height * 0.5f);

	m_WeaponRenders[WP_PISTOL].m_FlameDist = 1.0f;
	m_WeaponRenders[WP_ROCKET_LAUNCHER].m_FlameDist = 1.0f;
	m_WeaponRenders[WP_SHOTGUN].m_FlameDist = 1.0f;
	m_WeaponRenders[WP_AK47].m_FlameDist = PixelToMeter(m_WeaponRenders[WP_AK47].m_HandOffset + m_WpDims[WP_AK47].width) * 1.1f;
}

void Renderer::ProjectileSettings()
{
	m_aProjectileDim[WP_PISTOL].x = m_BulletW;
	m_aProjectileDim[WP_PISTOL].y = m_BulletH;
	m_aProjectileDim[WP_ROCKET_LAUNCHER].x = m_RocketW;
	m_aProjectileDim[WP_ROCKET_LAUNCHER].y = m_RocketH;
	m_aProjectileDim[WP_SHOTGUN].x = m_BulletW;
	m_aProjectileDim[WP_SHOTGUN].y = m_BulletH;
	m_aProjectileDim[WP_AK47].x = m_BulletW;
	m_aProjectileDim[WP_AK47].y = m_BulletH;

	m_aProjectileMappingIndex[WP_PISTOL] = PJ_BULLET;
	m_aProjectileMappingIndex[WP_ROCKET_LAUNCHER] = PJ_ROCKET;
	m_aProjectileMappingIndex[WP_SHOTGUN] = PJ_BULLET;
	m_aProjectileMappingIndex[WP_AK47] = PJ_BULLET;
}

void Renderer::InitTileDimensions()
{
	float ratio = static_cast<float>(Window::FrameWidth()) / Window::FrameHeight();
	m_iDrawnTilesHeight = 30;
	m_iDrawnTilesWidth = static_cast<int>(30.0f * ratio);
	m_HitBoxTileSize = static_cast<float>(Window::FrameHeight()) / static_cast<float>(m_iDrawnTilesHeight);
	SetDimensions(m_HitBoxTileSize);
	TileRender::m_TileSize = m_HitBoxTileSize;
}

void Renderer::SetDimensions(float tileSize)
{
	assert(tileSize > 0.0f);

	m_PixelsPerMeter = tileSize / AbsDim::TILE_SIZE;

	m_HitBoxCharW = MeterToPixel(AbsDim::CHAR_W);
	m_HitBoxCharH = MeterToPixel(AbsDim::CHAR_H);
	m_CharW = m_HitBoxCharW * 4.5f;
	m_CharH = m_HitBoxCharH * 1.6f;
	m_CharArmW = MeterToPixel(AbsDim::CHAR_ARM_W);
	m_CharArmH = MeterToPixel(AbsDim::CHAR_ARM_H);
	m_JetpackW = MeterToPixel(AbsDim::JETPACK_W);
	m_JetpackH = MeterToPixel(AbsDim::JETPACK_H);
	for (int i = 0; i < NUM_WEAPONS; ++i) {
		m_WpDims[i].width = MeterToPixel(AbsDim::WPDIMS[i].width);
		m_WpDims[i].height = MeterToPixel(AbsDim::WPDIMS[i].height);
	}
	m_HitBoxRocketW = MeterToPixel(AbsDim::ROCKET_W);
	m_HitBoxRocketH = MeterToPixel(AbsDim::ROCKET_H);
	m_RocketW = m_HitBoxRocketW*2.6f;
	m_RocketH = m_HitBoxRocketH*1.2f;
	m_HitBoxBulletW = MeterToPixel(AbsDim::BULLET_W);
	m_HitBoxBulletH = MeterToPixel(AbsDim::BULLET_H);
	m_BulletW =  m_HitBoxBulletW*10.0f;
	m_BulletH = m_HitBoxBulletH*1.5f;
	m_HitBoxTriggerW = MeterToPixel(AbsDim::TRIGGER_W);
	m_HitBoxTriggerH = MeterToPixel(AbsDim::TRIGGER_H);
	m_TriggerW = m_HitBoxTriggerW;
	m_TriggerH = m_HitBoxTriggerH;
}

void Renderer::RenderMap() const
{
	m_pMapRender->Render(m_Cam.GetPos());
}

void Renderer::RenderArm(CharacterRender *pCharRender, Vector2D weaponDir, Vector2D weaponPos, float weaponAngle)
{
		
	if (weaponDir.x < 0)
		pCharRender->m_ArmSprite.DirectToTop();
	else
		pCharRender->m_ArmSprite.DirectToBottom();

	// arm must go to back a little
	Vector2D offset = weaponDir * (m_CharArmW * 0.25f);

	// must give center of quad to rotate correctly
	pCharRender->m_ArmSprite.setPosWithAngle(weaponPos.x - m_CharArmW / 2, weaponPos.y - m_CharArmH / 2, weaponAngle, offset);
	Float2DArray mappings = GFXAsset::Instance().getTexMapping(MAP_CHAR_ARM);
	pCharRender->m_ArmSprite.setTexMapping(mappings[0]);
	pCharRender->m_ArmSprite.Render();
}

void Renderer::RenderJetpack(CharacterRender *pCharRender, Vector2D pos, Vector2D weaponDir)
{
	// centers on character
	pos.x += AbsDim::CHAR_W/2;

	// apply offset
	Vector2D offset(AbsDim::JETPACK_W * 0.25f , AbsDim::CHAR_H * 0.7f);
	if (weaponDir.x < 0) {
		pos.x += offset.x;
		pCharRender->m_JetpackSprite.DirectToLeft();
	}
	else {
		pos.x -= offset.x;
		pCharRender->m_JetpackSprite.DirectToRight();
	}
	pos.y += offset.y;

	pCharRender->m_JetpackSprite.SetPosition(MeterToPixel(pos));
	pCharRender->m_JetpackSprite.setCentered();
	Float2DArray mappings = GFXAsset::Instance().getTexMapping(MAP_JETPACK);
	pCharRender->m_JetpackSprite.setTexMapping(mappings[0]);
	pCharRender->m_JetpackSprite.Render();
}

float Renderer::GetLocalWeaponAngle(const CharacterData &localCharData) const
{
	Vector2D charPosInPixel(MeterToPixel(localCharData.pos));
	Vector2D weaponPos(charPosInPixel.x + m_HitBoxCharW*0.5f, charPosInPixel.y + m_HitBoxCharH*0.75f );
	Vector2D weaponDir (GetLocalWeaponDirection(weaponPos));
	return atan2f(weaponDir.y, weaponDir.x);
}

Vector2D Renderer::GetLocalWeaponDirection(Vector2D weaponPos) const {
	Vector2D camPos(m_Cam.GetPos());
	float kx = Window::m_RealFrameDims.x / Window::m_FrameWidth;
	float ky = Window::m_RealFrameDims.y / Window::m_FrameHeight;
	camPos.x *= kx;
	camPos.y *= ky;

	Vector2D relMousePos(InputManager::Instance().MouseX(), InputManager::Instance().MouseY());
	Vector2D absMousePos(camPos.x + relMousePos.x, camPos.y + Window::m_RealFrameDims.y - relMousePos.y); // thinking sdl (0,0) is top left

	weaponPos.x *= kx;
	weaponPos.y *= ky;

	Vector2D weaponDir = absMousePos - weaponPos;
	weaponDir.Normalize();

	return weaponDir;
}

Vector2D Renderer::GetWeaponDirection(Vector2D weaponPos, float weaponAngle, bool localPlayer) const
{
	Vector2D weaponDir;
	if (localPlayer)
	{
		weaponDir = GetLocalWeaponDirection(weaponPos);
	}
	else
	{
		weaponDir.x = cosf(weaponAngle);
		weaponDir.y = sinf(weaponAngle);
	}

	return weaponDir;
}

void Renderer::RenderTriggers(const TriggerData *pTrigData) const
{
	for(int i = 0; i < MAX_TRIGGERS; ++i)
	{
		if (!pTrigData[i].exist || !pTrigData[i].active)
			continue;

		Vector2D trigPosInPixel = MeterToPixel(pTrigData[i].pos);

		switch (pTrigData[i].type) {

		case TG_ITEM_POWERUP:
		{
			int powerupType = pTrigData[i].subType;
			assert(powerupType >= 0 && powerupType < NUM_POWERUPS);
			
			m_TriggerRenderPtrs[i]->m_Sprite.SetTextureId(TEX_POWERUPS);
			m_TriggerRenderPtrs[i]->m_Sprite.setSize(Vector2D(m_TriggerW, m_TriggerH));
			m_TriggerRenderPtrs[i]->m_Sprite.SetPosition(trigPosInPixel.x, trigPosInPixel.y);
			Float2DArray mappings = GFXAsset::Instance().getTexMapping(MAP_POWERUP_SET);
			m_TriggerRenderPtrs[i]->m_Sprite.setTexMapping(mappings[powerupType]);
			break;
		}

		case TG_ITEM_WEAPON:
		{
			int weaponType = pTrigData[i].subType;
			assert(weaponType >= 0 && weaponType < NUM_WEAPONS);

			m_TriggerRenderPtrs[i]->m_Sprite.SetTextureId(TEX_WEAPONS);
			m_TriggerRenderPtrs[i]->m_Sprite.setSize(Vector2D(m_WpDims[weaponType].width, m_WpDims[weaponType].height));
			m_TriggerRenderPtrs[i]->m_Sprite.SetPosition(trigPosInPixel.x, trigPosInPixel.y);
			Float2DArray mappings = GFXAsset::Instance().getTexMapping(MAP_WEAPON_SET);
			m_TriggerRenderPtrs[i]->m_Sprite.setTexMapping( mappings[weaponType] );
			m_TriggerRenderPtrs[i]->m_Sprite.setCentered();
			break;
		}

		default:
			assert(false && "wrong trigger type");
		}

		m_TriggerRenderPtrs[i]->m_Sprite.SetVisible();
		m_TriggerRenderPtrs[i]->m_Sprite.Render();
	}

}

void Renderer::RenderHitBox(Vector2D pos)
{
	m_HitBoxSprite.SetPosition(pos);
	m_HitBoxSprite.Render();
}

void Renderer::RenderBody(CharacterRender *pCharRender, Vector2D pos, int moveState, Vector2D weaponDir)
{
	pCharRender->m_BodySprite.SetPosition(MeterToPixel(pos));

	// direct to the good way
	if (weaponDir.x < 0)
		pCharRender->m_BodySprite.DirectToLeft();
	else
		pCharRender->m_BodySprite.DirectToRight();

	//TODO predicted locally
	switch (moveState)
	{
	case MOVE_JUMP:
	{
		pCharRender->m_JumpAnim.Play();

		if (!pCharRender->m_JumpSoundPlayed)
		{
			pCharRender->m_JumpSoundPlayed = true;
			pCharRender->m_LandingSoundPlayed = false;
			SoundPlayer::Instance().Play(CHN_WORLD, SND_JUMP, 2, pos);
		}

		break;
	}

	case MOVE_FLY:
	{
		pCharRender->m_JumpAnim.Play();
		pCharRender->m_JetpackSoundCustomId = SoundPlayer::Instance().Play(CHN_WORLD, SND_JETPACK, 2, pos, 64, pCharRender->m_JetpackSoundCustomId, true);
		pCharRender->m_JetpackSoundActive = true;
		break;
	}

	case MOVE_STATIONNARY:
	{
		pCharRender->m_StationaryAnim.Play();

		if (!pCharRender->m_LandingSoundPlayed)
		{
			pCharRender->m_JumpSoundPlayed = false;
			pCharRender->m_LandingSoundPlayed = true;
			SoundPlayer::Instance().Play(CHN_WORLD, SND_LANDING, 2, pos);
		}

		break;
	}

	case MOVE_FORWARD:
	{
		int stepIndex = pCharRender->m_WalkForwardAnim.Play();
		PlayFootstepSound(pCharRender, stepIndex, pos);
		break;
	}

	case MOVE_BACKWARD:
	{
		int stepIndex = pCharRender->m_WalkBackwardAnim.Play();
		PlayFootstepSound(pCharRender, stepIndex, pos);
		break;
	}

	default:
		assert(false && "Wrong move state !");
	}

	// stop jetpack sound
	if (moveState != MOVE_FLY && pCharRender->m_JetpackSoundActive) {
		SoundPlayer::Instance().Stop(pCharRender->m_JetpackSoundCustomId);
		pCharRender->m_JetpackSoundCustomId = INVALID_CUSTOM_ID;
		pCharRender->m_JetpackSoundActive = false;
	}

	pCharRender->m_BodySprite.Render();
}

void Renderer::RenderCharacter(int clientId, const CharacterData &charData, bool localPlayer)
{
	// Don't render what is not visible to the screen
	if(!m_Cam.isIntoFOV(charData.pos))
		return;

	if(charData.state == SPAWNING)
		return;

	Vector2D charPosInPixel = MeterToPixel(charData.pos);
	Vector2D weaponPos(charPosInPixel.x + m_HitBoxCharW*0.5f, charPosInPixel.y + m_HitBoxCharH*0.75f );
	Vector2D weaponDir = GetWeaponDirection(weaponPos, charData.weaponAngle, localPlayer);
	float weaponAngle = atan2f(weaponDir.y, weaponDir.x);
	CharacterRender *pCharRender = m_CharacterRenderPtrs[clientId];

	RenderBody(pCharRender, charData.pos, charData.moveState, weaponDir);
	RenderWeapon(pCharRender, charData.currentWeapon, weaponDir, weaponPos, weaponAngle);
	RenderJetpack(pCharRender, charData.pos, weaponDir);
	RenderArm(pCharRender, weaponDir, weaponPos, weaponAngle);

#if defined(DEBUG_RENDER)
	RenderHitBox(MeterToPixel(charData.pos));
#endif

}

void Renderer::RenderWeapon(CharacterRender *pCharRender, int currentWeaponType, Vector2D weaponDir, Vector2D weaponPos, float weaponAngle)
{
	assert ( currentWeaponType >= 0 && currentWeaponType < NUM_WEAPONS);

	// set size
	Vector2D size(m_WpDims[currentWeaponType].width, m_WpDims[currentWeaponType].height);
	pCharRender->m_WeaponSprite.setSize(size);
	pCharRender->m_WeaponSprite.setCentered();

	// place and rotate
	Vector2D offset (weaponDir * m_WeaponRenders[currentWeaponType].m_HandOffset);
	pCharRender->m_WeaponSprite.setPosWithAngle(weaponPos.x, weaponPos.y, weaponAngle, offset);

	// direct to the good way
	if( weaponDir.x < 0 ) { pCharRender->m_WeaponSprite.DirectToTop();}
	else { pCharRender->m_WeaponSprite.DirectToBottom();}

	// set the texture mapping
	Float2DArray mappings = GFXAsset::Instance().getTexMapping(MAP_WEAPON_SET);
	pCharRender->m_WeaponSprite.setTexMapping(mappings[currentWeaponType]);

	pCharRender->m_WeaponSprite.Render();
}

void Renderer::RenderProjectiles(const ProjectileData *projsData)
{
	glEnable(GL_TEXTURE_2D);
	GFXAsset::Instance().enableTex( TEX_PROJECTILES );

	for (int i = 0; i < MAX_PROJECTILES; ++i) {

		if (!projsData[i].exist)
			continue;

		int weaponType = projsData[i].weaponType;
		m_ProjectileSpritePtrs[i]->setSize( m_aProjectileDim[weaponType] );

		Vector2D projPosInPixel = MeterToPixel(projsData[i].pos);
		m_ProjectileSpritePtrs[i]->setPosWithAngle(projPosInPixel.x, projPosInPixel.y, projsData[i].angle);

		Float2DArray mappings = GFXAsset::Instance().getTexMapping(MAP_PROJECTILE_SET);
		int mappingIndex = m_aProjectileMappingIndex[weaponType];
		m_ProjectileSpritePtrs[i]->setTexMapping(mappings[mappingIndex]);
		m_ProjectileSpritePtrs[i]->SetVisible();
		m_ProjectileSpritePtrs[i]->Render(false);
	}

	glDisable(GL_TEXTURE_2D);
}


bool Renderer::LoadMap(const char *pMapName)
{
	assert(!m_pMapRender);
	m_pMapRender = new MapRender(m_iDrawnTilesWidth, m_iDrawnTilesHeight);
	return m_pMapRender->LoadMap(pMapName);
}

void Renderer::UnloadMap() {
	delete m_pMapRender;
	m_pMapRender = 0;
}

void Renderer::RenderChat(const Chat::TextMsgList & textMsgs)
{
	Vector2D origin(m_Cam.GetPos().x + 20, m_Cam.GetPos().y + 20);

	for (Chat::TextMsgList::const_iterator it = textMsgs.begin(); it != textMsgs.end(); ++it)
	{
		const Chat::TextMsg& msg = *it;
		TextRender::Instance().Render(origin.x, origin.y, msg.text, 1);
		origin.y += 20; //TODO put relative size
	}
}

void Renderer::PlayFootstepSound(CharacterRender *pCharRender, int stepIndex, Vector2D pos)
{
	if ( stepIndex == 4 && pCharRender->m_FootStepState == FOOTSTEP_HIGH )
	{
		int soundId = RandInt(SND_STEP_HIGH_1, SND_STEP_HIGH_4);
		SoundPlayer::Instance().Play(CHN_WORLD, soundId, 2, pos);
		pCharRender->m_FootStepState = FOOTSTEP_LOW;
	}
	else if (stepIndex == 9 && pCharRender->m_FootStepState == FOOTSTEP_LOW)
	{
		int soundId = RandInt(SND_STEP_LOW_1, SND_STEP_LOW_4);
		SoundPlayer::Instance().Play(CHN_WORLD, soundId, 2, pos);
		pCharRender->m_FootStepState = FOOTSTEP_HIGH;
	}
}

void Renderer::RenderEventSounds(const EventData *pEventData, ParticleManager &partMgr)
{
	for (int i = 0; i < EventData::numEvents; i++)
	{
		switch (pEventData[i].type)
		{
		case EVENT_FIRE_PISTOL:
		{
			SoundPlayer::Instance().Play(CHN_WORLD, SND_PISTOL_FIRE, 2, pEventData[i].pos);
			break;
		}

		case EVENT_FIRE_ROCKET_LAUNCHER:
		{
			SoundPlayer::Instance().Play(CHN_WORLD, SND_ROCKET_LAUNCHED, 2, pEventData[i].pos);
			break;
		}

		case EVENT_FIRE_AK47:
		{
			int sound = RandInt(SND_AK47_FIRE_1, SND_AK47_FIRE_3);
			SoundPlayer::Instance().Play(CHN_WORLD, sound, 2, pEventData[i].pos);
			break;
		}

		case EVENT_FIRE_SHOTGUN:
		{
			SoundPlayer::Instance().Play(CHN_WORLD, SND_SHOTGUN_FIRE, 2, pEventData[i].pos);
			break;
		}

		case EVENT_NO_AMMO:
		{
			int soundId = RandInt(SND_WEAPON_VOID_1, SND_WEAPON_VOID_2);
			SoundPlayer::Instance().Play(CHN_WORLD, soundId, 2, pEventData[i].pos);
			break;
		}

		case EVENT_ROCKET_EXPLODE:
		{
			SoundPlayer::Instance().Play(CHN_WORLD, SND_ROCKET_EXPLODE, 2, pEventData[i].pos);
			break;
		}

		case EVENT_ITEM_PICKUP:
		{
			SoundPlayer::Instance().Play(CHN_WORLD, SND_ITEM_PICKUP, 2, pEventData[i].pos);
			break;
		}

		case EVENT_PAIN:
		{
			int sndId = RandInt(SND_PAIN_1, SND_PAIN_2);
			SoundPlayer::Instance().Play(CHN_WORLD, SND_PAIN_1, 2, pEventData[i].pos);
			break;
		}

		case EVENT_DEATH:
		{
			break;
		}

		case EVENT_GIB_DEATH:
		{
			SoundPlayer::Instance().Play(CHN_WORLD, SND_GIB_BIG_SPLATTER, 2, pEventData[i].pos);
			break;
		}

		default:
			break;
		}
	}
}

void Renderer::SetLocalCharacterPos(Vector2D localCharPos)
{
	m_LocalCharPos = localCharPos;
}

void Renderer::ActivateConsole() {
	//	m_ConsoleRender.window->activate();
	m_ConsoleRender.window->setVisible(true);
	m_ConsoleRender.edit->activate();
}

void Renderer::DeactivateConsole() {
	m_ConsoleRender.window->setVisible(false);
	m_ConsoleRender.edit->deactivate();
}

void Renderer::UpdateConsole() {

	if (Console::Instance().HistoryChanged()) {
		m_ConsoleRender.history->setText(Console::Instance().GetHistory());
		int textLen = m_ConsoleRender.history->getText().length();
		m_ConsoleRender.history->setCaretIndex(textLen);
		m_ConsoleRender.history->ensureCaretIsVisible();
		Console::Instance().SetHistoryAsUnchanged();
	}
}

void Renderer::RenderHUD(const CharacterData &charData)
{
	m_pHudRender->m_vOrigin = m_Cam.GetPos();

	if (charData.state == ALIVE)
	{
		RenderHealthAndWeapon(charData.health, charData.currentWeapon, charData.aAmmoCount);
	}
}

void Renderer::RenderHealthAndWeapon(int health, int currentWeaponType, const int *pAmmoCount)
{
	float tsize = m_HitBoxTileSize;
	float x     = m_pHudRender->m_vOrigin.x + Window::FrameWidth()*0.70f;
	float y     = m_pHudRender->m_vOrigin.y + tsize;

	// Health

	m_pHudRender->m_healthPanel.Render( x-tsize/2, y-tsize, 0.26f,0.18f,0.12f, 0.3f);
	m_pHudRender->m_sprHealth.SetPosition( x+tsize/2, y );
	m_pHudRender->m_sprHealth.Render();
	x += 1.5*tsize;

	Clamp(health, 0, 100);
	char healthStr[4];
	std::sprintf(healthStr, "%d", health);

	TextRender::Instance().Render( x , y, healthStr, 3);

	// Weapon

	x += 3.5*tsize;
	m_pHudRender->m_weaponPanel.Render(x-tsize/2, y-tsize, 0.26f,0.18f,0.12f, 0.3f);

	x += 1.5*tsize;
	m_pHudRender->m_sprWeapon.SetPosition(x, y);

	Float2DArray mapping = GFXAsset::Instance().getTexMapping( MAP_WEAPON_SET );
	m_pHudRender->m_sprWeapon.setTexMapping( mapping[currentWeaponType] );

	// take relative dimensions of pistol to compute others
	Vector2D wpSize (tsize, tsize/2);
	wpSize *= 1.5f;

	switch(currentWeaponType)
	{
	case WP_PISTOL:
		break;

	case WP_ROCKET_LAUNCHER:
		wpSize.x *= 1.5f;
		wpSize.y *= 1.5f;
		break;

	case WP_AK47:
		wpSize.x *= 1.8f;
		break;

	case WP_SHOTGUN:
		wpSize.x *= 1.9f;
		break;

	default:
		assert(false && "wrong weapon type");
		break;
	}

	m_pHudRender->m_sprWeapon.setSize(wpSize);
	m_pHudRender->m_sprWeapon.Render();

	// weapon counter
	x += 2*tsize;
	char ammoCountStr[10];
	std::sprintf(ammoCountStr, "%d", pAmmoCount[currentWeaponType]);
	TextRender::Instance().Render( x , y, ammoCountStr, 3);
}

void Renderer::RenderScoreBoard(const std::vector<CharacterData> &charData, int localClientId)
{
	float tsize = m_HitBoxTileSize;

	// origin of score board
	float x = m_pHudRender->m_vOrigin.x + Window::FrameWidth()/2 - m_pHudRender->m_bgScorePanel.Width()/2;
	float y = m_pHudRender->m_vOrigin.y + Window::FrameHeight()/2 - m_pHudRender->m_bgScorePanel.Height()/2;

	m_pHudRender->m_bgScorePanel.Render(x, y, 00.26f,0.18f,0.12f, 0.3f);

	// Score board title
	x += tsize;
	y += m_pHudRender->m_bgScorePanel.Height() - tsize;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	TextRender::Instance().Render( x , y, "Score Board", 5);

	// Score of player
	TextRender::Instance().Render( x + m_pHudRender->m_bgScorePanel.Width() - 3*tsize, y, "999", 6);

	// Score board column titles
	float xName = x;
	float xKill = xName + 10*tsize;
	float xDeath = xKill + 3*tsize;

	y -= 1.5*tsize;
	TextRender::Instance().Render( xName, y, "Name", 3);
	TextRender::Instance().Render( xKill, y, "Kill", 3);
	TextRender::Instance().Render( xDeath, y, "Death", 3);

	// All scores
	for (int clientId = 0; clientId < MAX_CLIENTS; clientId++)
	{
		if (charData[clientId].state == INACTIVE)
			continue;

		y -= 2*tsize;

		if (clientId == localClientId)
		{
			m_pHudRender->m_highlightPanel.Render(x-tsize/2, y-tsize, 0.39f, 0.26f, 0.17f, 0.5f);
		}

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		char name[MAX_PLAYERNAME_LENGTH];
		std::sprintf(name, charData[clientId].name, clientId);
		char kills[10];
		std::sprintf(kills, "%d", charData[clientId].kills);
		char deaths[10];
		std::sprintf(deaths, "%d", charData[clientId].deaths);

		TextRender::Instance().Render( xName, y, name, 2);
		TextRender::Instance().Render( xKill, y, kills, 2);
		TextRender::Instance().Render( xDeath, y, deaths, 2);
	}
}

void Renderer::BeginMenu() {
	ClearBuffer();
	SetCamera(Vector2D(0,0));
	InitRender();
	//	RenderBackgroundMenu();
	m_MenuBackgroundSprite.Render();
}

void Renderer::EndMenu() {
	SwapBuffer();
}

void Renderer::RenderBackgroundMenu()
{
	m_MenuBackgroundSprite.SetVisible();
	m_MenuBackgroundSprite.setTexMapping();
	m_MenuBackgroundSprite.setCentered();
	m_MenuBackgroundSprite.SetPosition(0,0);
}

void Renderer::RenderMapBackground() {
	m_BackgroundSprite.SetVisible();
	m_BackgroundSprite.setTexMapping();
	m_BackgroundSprite.SetPosition(m_Cam.GetPos());
	m_BackgroundSprite.Render();
}

const Renderer::WeaponRender & Renderer::GetWeaponRender(int type)
{
	return m_WeaponRenders[type];
}


void Renderer::UpdateSkin(int clientId, int skinId)
{
	assert(skinId >= 0 && skinId < NUM_SKINS);

	int texId = TEX_PLAYER_0 + skinId;
	m_CharacterRenderPtrs[clientId]->m_BodySprite.SetTextureId(texId);
	m_CharacterRenderPtrs[clientId]->m_ArmSprite.SetTextureId(texId);
}

void Renderer::InitCEGUI() {

	printf("init renderer\n");
	m_CEGUI_Renderer = &CEGUI::OpenGLRenderer::create();

	if (!m_CEGUI_Renderer)
		throw CEGUI::InvalidRequestException("CEGuiBaseApplication::run: Base application subclass did not create Renderer!");

	

	// initialise the required dirs for the DefaultResourceProvider
	printf("init system\n");



	// start up CEGUI system using objects created in subclass constructor.
	CEGUI::System::create(*m_CEGUI_Renderer, 0, 0, 0);

	printf("init resource system\n");
	// initialise resource system

	InitialiseResourceGroupDirectories();
	InitialiseDefaultResourceGroups();

	printf("init scheme manager\n");
	CEGUI::SchemeManager::getSingleton().createFromFile("RouageLook.scheme");

	printf("init GUI context\n");
	CEGUI::GUIContext *guiContext = &CEGUI::System::getSingleton().getDefaultGUIContext();

	guiContext->getMouseCursor().setDefaultImage("RouageLook/MouseArrow");

	printf("init Window manager\n");
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

	printf("init font\n");

	CEGUI::Font& defaultFont = CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-10.font");
	CEGUI::Font& bigFont = CEGUI::FontManager::getSingleton().createFromFile("VanillaWhale-Big.font");
	CEGUI::Font& mediumFont = CEGUI::FontManager::getSingleton().createFromFile("VanillaWhale-Medium.font");
	CEGUI::Font& smallFont = CEGUI::FontManager::getSingleton().createFromFile("VanillaWhale-Small.font");

	m_CEGUI_Fonts[CEGUI_FONT_DEFAULT] = &defaultFont;
	m_CEGUI_Fonts[CEGUI_FONT_MENU_TITLE] = &bigFont;
	m_CEGUI_Fonts[CEGUI_FONT_MENU_LABEL] = &mediumFont;
	m_CEGUI_Fonts[CEGUI_FONT_MENU_BUTTON] = &mediumFont;
	m_CEGUI_Fonts[CEGUI_FONT_CONSOLE_HISTORY] = &defaultFont;
	m_CEGUI_Fonts[CEGUI_FONT_CONSOLE_BUTTON] = &smallFont;

	m_CEGUI_MenuWindow = winMgr.createWindow("DefaultWindow", "MenuWindow");
	m_CEGUI_MenuWindow->setFont(&mediumFont);
	m_CEGUI_GameWindow = winMgr.createWindow("DefaultWindow", "GameWindow");
	//	m_CEGUI_GameWindow->setFont(&mediumFont);

	guiContext->setDefaultFont(&mediumFont);
	guiContext->setRootWindow(m_CEGUI_MenuWindow);

	InitConsoleRender();
	m_CEGUI_GameWindow->addChild(m_ConsoleRender.window);

	InitChatRender();
	m_CEGUI_GameWindow->addChild(m_ChatRender.window);

}

void Renderer::InitialiseResourceGroupDirectories()
{
	CEGUI::DefaultResourceProvider* rp =
			static_cast<CEGUI::DefaultResourceProvider*>
	(CEGUI::System::getSingleton().getResourceProvider());

	const char* dataPathPrefix = "data/cegui/";
	char resourcePath[1024];

	// for each resource type, set a resource group directory
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "schemes/");
	rp->setResourceGroupDirectory("schemes", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "imagesets/");
	rp->setResourceGroupDirectory("imagesets", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "fonts/");
	rp->setResourceGroupDirectory("fonts", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "layouts/");
	rp->setResourceGroupDirectory("layouts", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "looknfeel/");
	rp->setResourceGroupDirectory("looknfeels", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "lua_scripts/");
	rp->setResourceGroupDirectory("lua_scripts", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "xml_schemas/");
	rp->setResourceGroupDirectory("schemas", resourcePath);
	sprintf(resourcePath, "%s/%s", dataPathPrefix, "animations/");
	rp->setResourceGroupDirectory("animations", resourcePath);
}

void Renderer::InitialiseDefaultResourceGroups()
{
	// set the default resource groups to be used
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	CEGUI::AnimationManager::setDefaultResourceGroup("animations");

	// setup default group for validation schemas
	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
	if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");
}

void Renderer::InitConsoleRender() {

	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::DefaultWindow *window = static_cast<CEGUI::DefaultWindow*>(winMgr.loadLayoutFromFile("console.layout"));
	window->setText("Console");
	m_ConsoleRender.window = window;

	CEGUI::MultiLineEditbox *history = static_cast<CEGUI::MultiLineEditbox*>(window->getChild("History"));
	history->setFont(m_CEGUI_Fonts[CEGUI_FONT_DEFAULT]);
	m_ConsoleRender.history = history;

	CEGUI::Editbox *edit = static_cast<CEGUI::Editbox*>(window->getChild("EditGroup/Edit"));
	edit->setFont(m_CEGUI_Fonts[CEGUI_FONT_DEFAULT]);
	edit->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&ConsoleRender::SendCommandByReturn, &this->m_ConsoleRender));
	m_ConsoleRender.edit = edit;

	CEGUI::ButtonBase *submit = static_cast<CEGUI::ButtonBase*>(window->getChild("EditGroup/Submit"));
	submit->setText("Submit");
	submit->setFont(m_CEGUI_Fonts[CEGUI_FONT_CONSOLE_BUTTON]);
	submit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ConsoleRender::SendCommandByButton, &this->m_ConsoleRender));

	DeactivateConsole();
}

void Renderer::InitChatRender() {

	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::DefaultWindow *window = static_cast<CEGUI::DefaultWindow*>(winMgr.loadLayoutFromFile("chat.layout"));
	window->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&ChatRender::Unfocus, &this->m_ChatRender));
	m_ChatRender.window = window;

	CEGUI::Listbox *history = static_cast<CEGUI::Listbox*>(window->getChild("History"));
	history->setFont(m_CEGUI_Fonts[CEGUI_FONT_DEFAULT]);
	history->getVertScrollbar()->setAlpha(0);
	history->getHorzScrollbar()->setAlpha(0);
	m_ChatRender.history = history;

	CEGUI::Editbox *edit = static_cast<CEGUI::Editbox*>(window->getChild("Edit"));
	edit->setFont(m_CEGUI_Fonts[CEGUI_FONT_DEFAULT]);
	edit->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&ChatRender::SendCommandByReturn, &this->m_ChatRender));
	m_ChatRender.edit = edit;

}

void Renderer::RenderStartMenu(Menu *menu) {
	BeginMenu();
	RenderUI();
	EndMenu();
}

void Renderer::RenderUI() {
	CEGUI::System::getSingleton().renderAllGUIContexts();
}

void Renderer::AddWindowInMenu(CEGUI::Window *w) {
	m_CEGUI_MenuWindow->addChild(w);
}

void Renderer::AddWindowInGame(CEGUI::Window *w) {
	m_CEGUI_GameWindow->addChild(w);
}

void Renderer::ActiveMenuWindow() {
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(m_CEGUI_MenuWindow);
}

void Renderer::ActiveGameWindow() {
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(m_CEGUI_GameWindow);
}

void Renderer::AddChatMessage(const char *msg) {

	CEGUI::ListboxTextItem *newItem=0;
	newItem = new CEGUI::FormattedListboxTextItem(msg, CEGUI::HTF_WORDWRAP_LEFT_ALIGNED);
	newItem->setAutoDeleted(true);
	m_ChatRender.history->addItem(newItem);

	int msgCount = m_ChatRender.history->getItemCount();
	if (msgCount > ChatRender::MAX_CHAT_MSG) {
		m_ChatRender.history->removeItem( m_ChatRender.history->getListboxItemFromIndex(0) );
	}

	m_ChatRender.history->ensureItemIsVisible(msgCount-1);
}

bool Renderer::FocusOnChat() const {
	return m_ChatRender.edit->isActive();
}

void Renderer::SetFocusOnChat(bool focus) {
	if (focus)
		m_ChatRender.edit->activate();
	else
		m_ChatRender.edit->deactivate();
}

void Renderer::SetChatVisible(bool b) {
	m_ChatRender.window->setVisible(b);
}

void Renderer::ToggleChat() {
	bool visible = m_ChatRender.window->isVisible();
	if (visible && m_ChatRender.edit->isActive())
		return;
	m_ChatRender.window->setVisible(!visible);
}

void Renderer::ToggleInGameMenu() {
	CEGUI::Window *inGameMenu = m_CEGUI_GameWindow->getChild("InGameMenu");
	bool visible = inGameMenu->isVisible();
	inGameMenu->setVisible(!visible);

	if (visible)
		inGameMenu->deactivate();
	else
		inGameMenu->activate();
}


bool Renderer::InGameMenu() const {
	return m_CEGUI_GameWindow->getChild("InGameMenu")->isActive();
}

void Renderer::ChangeResolution(int w, int h) {
	Window::ChangeResolution(w, h);
	ResizeBackground();
}
