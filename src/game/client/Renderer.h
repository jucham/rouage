#ifndef RENDER_H_
#define RENDER_H_

#include <list>
#include <engine/shared/AbsDim.h>
#include <game/server/Weapon.h>
#include <base/Vector2D.h>
#include <engine/client/Window.h>
#include <engine/client/gfx/SpriteAnimation.h>
#include <engine/client/gfx/VBOSprite.h>
#include <engine/client/gfx/GFXAsset.h>
#include <engine/client/sfx/SoundPlayer.h>
#include "Camera.h"
#include "Chat.h"
#include <engine/client/gfx/Panel.h>
#include <engine/client/CommandManager.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GLRenderer.h>
#include "ui/Menu.h"

#include <engine/client/InputManager.h>

class MapRender;
struct CharacterData;
struct ProjectileData;
struct EventData;
struct TriggerData;
class InputManager;
class GameClient;
class ParticleManager;

class Renderer
{
public:

	enum {
		CEGUI_FONT_DEFAULT,
		CEGUI_FONT_MENU_TITLE,
		CEGUI_FONT_MENU_LABEL,
		CEGUI_FONT_MENU_BUTTON,
		CEGUI_FONT_CONSOLE_HISTORY,
		CEGUI_FONT_CONSOLE_BUTTON,
		NUM_CEGUI_FONTS
	};

	struct WeaponRender {
		float m_HandOffset;
		float m_EjectionOffset;
		float m_ProjCannonOffset;
		float m_FlameDist;
	};

	Renderer(GameClient &gameClient);
	static Vector2D MeterToPixel(Vector2D v) { return v * m_PixelsPerMeter; }
	static float MeterToPixel(float v) { return v * m_PixelsPerMeter; }
	static Vector2D PixelToMeter(Vector2D v) { return v * (1.0f / m_PixelsPerMeter); }
	static float PixelToMeter(float v) { return v * (1.0f / m_PixelsPerMeter); }
	void SetCamera(Vector2D playerPos);
	bool LoadMap(const char *pMapName);
	void UnloadMap();
	void RenderMap() const;
	void RenderTriggers(const TriggerData *pTriggerData) const;
	void RenderCharacter(int clientId, const CharacterData &charData, bool isLocalPlayer);
	void RenderProjectiles(const ProjectileData *pProjData);
	void RenderParticules();
	void ClearBuffer() const { glClear(GL_COLOR_BUFFER_BIT); }
	void InitRender() const { glMatrixMode(GL_MODELVIEW); }
	static void SwapBuffer() { Window::Swap(); }
	float GetLocalWeaponAngle(const CharacterData &charData) const;
	const Camera &GetCam() const;
	void RenderChat(const Chat::TextMsgList & textMsgs);
	void RenderEventSounds(const EventData *pEventData, ParticleManager &partMgr);
	void RenderHUD(const CharacterData &charData);
	void RenderScoreBoard(const std::vector<CharacterData> &charData, int localClientId);
	void UpdateConsole();
	void BeginMenu();
	void EndMenu();
	void RenderBackgroundMenu();
	void RenderMapBackground();
	void SetLocalCharacterPos(Vector2D localCharPosInMeter);
	const WeaponRender & GetWeaponRender(int type);
	void UpdateSkin(int clientId, int skinId);
	inline MapRender & GetMapRender();
	void RenderStartMenu(Menu *menu);
	void AddWindowInMenu(CEGUI::Window* w);
	void AddWindowInGame(CEGUI::Window *w);
	void ActivateConsole();
	void DeactivateConsole();
	void RenderUI();
	void ActiveMenuWindow();
	void ActiveGameWindow();
	const CEGUI::Font * GetCEGUIFont(int fontId) {return m_CEGUI_Fonts[fontId];}
	void AddChatMessage(const char *msg);
	bool FocusOnChat() const;
	void SetFocusOnChat(bool b);
	void SetChatVisible(bool b);
	void ToggleChat();
	void ToggleInGameMenu();
	bool InGameMenu() const;
	void InitTileDimensions();
	void ChangeResolution(int w, int h);

	static float m_PixelsPerMeter; // one meter in pixel unit

private:

	int m_iDrawnTilesWidth;
	int m_iDrawnTilesHeight;

	float m_HorStretchFactor;
	float m_HitBoxTileSize;
	float m_HitBoxCharW;
	float m_HitBoxCharH;
	float m_CharW;
	float m_CharH;
	float m_CharArmW;
	float m_CharArmH;
	float m_JetpackW;
	float m_JetpackH;
	Dimension m_WpDims[NUM_WEAPONS];
	float m_HitBoxRocketW;
	float m_HitBoxRocketH;
	float m_RocketW;
	float m_RocketH;
	float m_HitBoxBulletW;
	float m_HitBoxBulletH;
	float m_BulletW;
	float m_BulletH;
	float m_HitBoxTriggerW;
	float m_HitBoxTriggerH;
	float m_TriggerW;
	float m_TriggerH;
	float m_PowerupW;
	float m_PowerupH;

	MapRender *m_pMapRender;
	VBOSprite m_HitBoxSprite;

	struct TriggerRender {
		VBOSprite m_Sprite;

		TriggerRender() :
			m_Sprite(TEX_NULL, GL_STREAM_DRAW, GL_STREAM_DRAW, GL_STREAM_DRAW, 0, 0)
		{
			//			assert (weaponType >= 0 && weaponType < NUM_WEAPONS);
			//
			//			Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_WEAPON_SET);
			//			m_Sprite.setTexMapping(mapping[weaponType]);
			//			m_Sprite.SetVisible();
			//			m_Sprite.setOffset(-(m_WpDims[weaponType].width-tileSize)/2, 0);
		}
	};
	std::vector<TriggerRender*> m_TriggerRenderPtrs;

	//	void RenderWeaponItem(const TriggerData &trigData, int weaponItemIndex) const;
	//	void RenderPowerupItem(const TriggerData &trigData, int powerupItemIndex) const;

	struct CharacterRender {
		// GFX
		static const int WALK_SPRITE_DISPLAY_TIME = 60;
		VBOSprite m_BodySprite;
		VBOSprite m_ArmSprite;
		VBOSprite m_WeaponSprite;
		VBOSprite m_JetpackSprite;
		SpriteAnimation m_StationaryAnim;
		SpriteAnimation m_WalkForwardAnim;
		SpriteAnimation m_WalkBackwardAnim;
		SpriteAnimation m_JumpAnim;

		// SFX
		int m_FootStepState;
		bool m_JumpSoundPlayed;
		bool m_LandingSoundPlayed;
		unsigned int m_JetpackSoundCustomId;
		bool m_JetpackSoundActive;

		CharacterRender(Vector2D charDim, Vector2D charOffset, Vector2D armDim, Vector2D jetpackDim, const GFXAsset &gfxAsset) :
			m_BodySprite(TEX_PLAYER_0, GL_STREAM_DRAW, GL_STREAM_DRAW, GL_STATIC_DRAW, charDim.x, charDim.y, charOffset),
			m_ArmSprite(TEX_PLAYER_0, GL_STREAM_DRAW, GL_STREAM_DRAW, GL_STATIC_DRAW, armDim.x, armDim.y),
			m_WeaponSprite(TEX_WEAPONS, GL_STREAM_DRAW, GL_STREAM_DRAW, GL_STATIC_DRAW, 0.0f, 0.0f),
			m_JetpackSprite(TEX_EQUIPMENT, GL_STREAM_DRAW, GL_STREAM_DRAW, GL_STATIC_DRAW, jetpackDim.x, jetpackDim.y),
			m_StationaryAnim(gfxAsset, 0, &m_BodySprite, MAP_CHAR_STATIONARY),
			m_WalkForwardAnim(gfxAsset, WALK_SPRITE_DISPLAY_TIME, &m_BodySprite, MAP_CHAR_WALKFORWARDSET),
			m_WalkBackwardAnim(gfxAsset, WALK_SPRITE_DISPLAY_TIME, &m_BodySprite, MAP_CHAR_WALKBACKWARDSET),
			m_JumpAnim(gfxAsset, 0, &m_BodySprite, MAP_CHAR_JUMP),
			m_FootStepState(FOOTSTEP_HIGH),
			m_JumpSoundPlayed(true),
			m_LandingSoundPlayed(false),
			m_JetpackSoundCustomId(0),
			m_JetpackSoundActive(false)
		{}
	};
	std::vector<CharacterRender*> m_CharacterRenderPtrs;

	std::vector<VBOSprite*> m_ProjectileSpritePtrs;
	Vector2D m_aProjectileDim[NUM_WEAPONS];
	int m_aProjectileMappingIndex[NUM_WEAPONS];

	Camera m_Cam;
	WeaponRender m_WeaponRenders[NUM_WEAPONS];

	//	void InitTileDimensions();
	void SetDimensions(float tileSize);
	void CreateCharacterRenders();
	Vector2D GetWeaponDirection(Vector2D weaponPos, float weaponAngle, bool localPlayer) const;
	Vector2D GetLocalWeaponDirection(Vector2D weaponPos) const;
	void RenderBody(CharacterRender *pCharRender, Vector2D pos, int moveState, Vector2D weaponDir);
	void RenderWeapon(CharacterRender *pCharRender, int currentWeaponIdx, Vector2D facing, Vector2D weaponPos, float weaponAngle);
	void RenderArm(CharacterRender *pCharRender, Vector2D facing, Vector2D weaponPos, float weaponAngle);
	void RenderJetpack(CharacterRender *pCharRender, Vector2D pos, Vector2D weaponDir);
	void RenderHitBox(Vector2D pos);
	void RenderHealthAndWeapon(int health, int currentWeaponType, const int *pAmmoCount);
	void CreateProjectileSprites();
	void WeaponSettings();
	void ProjectileSettings();

	GameClient &m_GameClient;
	Vector2D m_LocalCharPos;

	void PlayFootstepSound(CharacterRender *pCharRender, int stepIndex, Vector2D pos);
	void CreateTriggerRenders();

	enum {
		FOOTSTEP_HIGH,
		FOOTSTEP_LOW
	};

	//TODO put the scoreboard apart
	struct HudRender
	{
		Vector2D m_vOrigin;
		VBOSprite m_sprHealth;
		VBOSprite m_sprWeapon;
		Panel m_healthPanel;
		Panel m_weaponPanel;
		Panel m_bgScorePanel;
		Panel m_highlightPanel;

		HudRender(int hitBoxTileSize) :
			m_vOrigin(),
			m_sprHealth(TEX_HUD, GL_STREAM_DRAW, GL_STATIC_DRAW, GL_STATIC_DRAW, hitBoxTileSize, hitBoxTileSize),
			m_sprWeapon(TEX_WEAPONS, GL_STREAM_DRAW, GL_STREAM_DRAW, GL_STATIC_DRAW, hitBoxTileSize, hitBoxTileSize),
			m_healthPanel( 3.5*hitBoxTileSize, 2*hitBoxTileSize, 15, 15),
			m_weaponPanel( 6*hitBoxTileSize, 2*hitBoxTileSize, 15, 15),
			m_bgScorePanel( 20*hitBoxTileSize, 15*hitBoxTileSize, 15, 15),
			m_highlightPanel( 19*hitBoxTileSize, 2*hitBoxTileSize, 10, 10)
		{
			Float2DArray mapping = GFXAsset::Instance().getTexMapping(MAP_HUD_HEALTH);
			m_sprHealth.setTexMapping( mapping[0] );
			m_sprHealth.setCentered();
			m_sprHealth.SetVisible();
			m_sprWeapon.setCentered();
			m_sprWeapon.SetVisible();
		}
	};
	HudRender *m_pHudRender;

	struct ConsoleRender {
		CEGUI::Window *window;
		CEGUI::MultiLineEditbox *history;
		CEGUI::Editbox *edit;

		ConsoleRender() : window(0), history(0), edit(0) {}

		bool SendCommandByReturn(const CEGUI::EventArgs& ea) {
			SendCommand();
			return true;
		}

		bool SendCommandByButton(const CEGUI::EventArgs& ea) {
			SendCommand();
			return true;
		}

		void SendCommand() {
			const char *cmd = edit->getText().c_str();
			CommandManager::Instance().AddCommand(cmd);
			edit->setText("");
		}
	} m_ConsoleRender;

	struct ChatRender {

		static const int MAX_CHAT_MSG = 100;
		CEGUI::Window *window;
		CEGUI::Listbox *history;
		CEGUI::Editbox *edit;

		ChatRender() : window(0), history(0), edit(0) {}

		bool SendCommandByReturn(const CEGUI::EventArgs& ea) {
			SendCommand();
			return true;
		}

		void SendCommand() {
			const char *msg = edit->getText().c_str();

			if (!msg || std::strlen(msg) == 0) {
				return;
			}
			//TODO check size of msg
			char cmd[1024];
			//			sprintf(cmd, "say [colour='FFFF0000']%s", msg);
			sprintf(cmd, "say %s", msg);
			CommandManager::Instance().AddCommand(cmd);
			edit->setText("");
		}

		bool Unfocus(const CEGUI::EventArgs& ea) {
			edit->deactivate();
			return true;
		}

	} m_ChatRender;

	VBOSprite m_StartscreenSprite;
	VBOSprite m_BackgroundSprite;
	VBOSprite m_MenuBackgroundSprite;

	CEGUI::Renderer *m_CEGUI_Renderer;
	CEGUI::Window *m_CEGUI_MenuWindow;
	CEGUI::Window *m_CEGUI_GameWindow;
	CEGUI::Font *m_CEGUI_Fonts[NUM_CEGUI_FONTS];

	void InitCEGUI();
	void InitialiseResourceGroupDirectories();
	void InitialiseDefaultResourceGroups();
	void InjectTimePulse();
	void InitConsoleRender();
	void InitChatRender();
	void ResizeBackground();
};


inline void Renderer::SetCamera(Vector2D playerPos) {
	glViewport(0, 0,  Window::m_RealFrameDims.x,  Window::m_RealFrameDims.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float xScale = Window::m_RealFrameDims.x / Window::m_FrameWidth;
	float yScale = Window::m_RealFrameDims.y / Window::m_FrameHeight;
	// useful to prevent stretching on x
	float ratioFactor = (static_cast<float>(Window::FrameWidth()) / Window::FrameHeight()) / (Window::m_RealFrameDims.x / Window::m_RealFrameDims.y);
	glScalef(xScale * ratioFactor, yScale, 1);
	glTranslatef(1.0f - 1.0f/xScale, 1.0f - 1.0f/yScale, 1.0f);

	Vector2D playerPosInPixel = MeterToPixel(playerPos);
	m_Cam.setPositionX(playerPosInPixel.x - Window::m_FrameWidth/2.0f);
	m_Cam.setPositionY(playerPosInPixel.y - Window::m_FrameHeight/2.0f);
	float bottomLeftX = m_Cam.GetPos().x;
	float topRightX = Window::m_RealFrameDims.x + m_Cam.GetPos().x;
	float bottomLeftY = m_Cam.GetPos().y;
	float topRightY = Window::m_RealFrameDims.y + m_Cam.GetPos().y;

	// sets up a two-dimensional orthographic viewing region with camera's coordinates
	glOrtho(bottomLeftX, topRightX, bottomLeftY, topRightY, -1, 1);
}

inline const Camera & Renderer::GetCam() const {return m_Cam;}
inline MapRender & Renderer::GetMapRender() {return *m_pMapRender;}

#endif /* RENDER_H_ */
