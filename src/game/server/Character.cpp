#include "Character.h"
#include <sstream>
#include <cstring>
#include "GameServer.h"
#include "Map.h"
#include "Weapon.h"
#include "WeaponSystem.h"
#include "MovingEntity.h"
#include "Tile.h"
#include "Projectile.h"
//#include "Corpse.h"
#include <base/debug.h>
#include <base/Time.h>
#include <base/tri_logger.hpp>


float Character::m_fWIDTH;
float Character::m_fHEIGHT;
float Character::m_fARM_WIDTH;
float Character::m_fARM_HEIGHT;
const float Character::MAX_DO_JUMP_TIME = 0.3f;
const float Character::AIR_FRICTION_FACTOR = 0.95f;


Character::Character(GameServer *pGameServer, int clientId) :
		MovingEntity(pGameServer, 0, Vector2D(), 0.8f),
		m_State(INACTIVE),
		m_MoveState(MOVE_STATIONNARY),
		m_Health(100),
		m_MaxHealth(100),
		m_Jumping(false),
		m_bJumped(false),
		m_Falling(false),
		m_bFallen(false),
		m_SideVelocity(0),
		m_InitJumpVelocity(0),
		m_JetpackVelocity(0),
		m_pWeaponSys(0),
		m_DeathTime(-1),
		m_ClientId(clientId),
		m_KillerClientId(INVALID_CLIENT_ID),
		m_JumpReleased(true),
		m_StartJumpingTime(NOT_INITIALIZED_VALUE),
		m_OwnJetpack(true),
		m_Jetpacking(false),
		m_StartJetpackingTime(0),
		m_JetpackFallVelocity(0.0f)
{
	float velocity = 5.5f; // m/s
	m_SideVelocity = velocity * 3/100;
	m_InitJumpVelocity = 0.2f;
	m_JetpackVelocity = m_InitJumpVelocity * 0.9f;

	// sets bounding box
	m_HitBox.setWidth(m_fWIDTH);
	m_HitBox.setHeight(m_fHEIGHT);

	assert(m_HitBox.Width() < Tile::m_fWIDTH * 2 && m_HitBox.Height() < Tile::m_fHEIGHT * 4);

	std::memset(m_aInputs, 0, NUM_INPUTS);
}

Character::~Character() {
	delete m_pWeaponSys;
}


int Character::GetMaxLife() const {
	return m_MaxHealth;
}

void Character::addLife(int n) {
	if (m_Health + n > m_MaxHealth) {
		m_Health = m_MaxHealth;
	} else {
		m_Health += n;
	}
}

void Character::removeLife(int n) {
	m_Health -= n;
}

void Character::Update() {
	updateSpecial();

	if (m_aInputs[INPUT_JUMP] == 0) {
		m_JumpReleased = true;
	}

	// avoid input repeats
	m_aInputs[INPUT_ATTACK] = 0;
	m_aInputs[INPUT_PREV_WEAPON] = 0;
	m_aInputs[INPUT_NEXT_WEAPON] = 0;
}

void Character::UpdatePos() {

	// --- JETPACKING ---

	if (m_OwnJetpack && IsJetpacking()) {

		// take into account the vertical force when falling
		// to avoid to going up directly
		if ((m_Jumping || m_Falling) && m_vVelocity.y < 0) {
			m_StartJetpackingTime = Time::ms();
			m_JetpackFallVelocity = m_vVelocity.y;
		}

		m_Jumping = false;
		m_Falling = false;
		m_vVelocity.y = m_JetpackVelocity + m_JetpackFallVelocity;

		// updates fall velocity
		float jetpackingTime = (Time::ms()-m_StartJetpackingTime)/1000.0f;
		const float TWEAK_FACTOR = 0.5f;
		m_JetpackFallVelocity += m_fG * jetpackingTime * TWEAK_FACTOR;
		if (m_JetpackFallVelocity > 0.0f)
			m_JetpackFallVelocity = 0.0f;
	}

	if (m_JumpReleased && isGround() && isDoingJump()) {
		m_Jumping = true;
	}

	// --- FALLING ---

	if (m_Falling) {

		float fallingTime; // time elapsed between fly starting time and current fly time
		int curTime = Time::ms();

		// first passing here, some things must be initialized, so character "isn't still fallen"
		if (!m_bFallen) {
			m_iStartFlyingTime = curTime;
			fallingTime = 0.0f;
			m_bFallen = true;
		} else {
			fallingTime = (curTime - m_iStartFlyingTime) / 1000.0f;
		}

		m_vVelocity.y = -m_fG * fallingTime;
	}

	// --- JUMPING ---

	if (m_Jumping) {

		float jumpingTime; // time elapsed between fly starting time and current fly time
		int curTime = Time::ms();
		float jumpVelocity = m_InitJumpVelocity;

		// first passing here, some things must be initialized, so character "isn't still jumped"
		if (!m_bJumped) {

			m_JumpReleased = false;
			m_iStartFlyingTime = curTime;
			jumpingTime = 0.0f;

			// player press jump so character gets a little boost while jump is pressed.
			// the boost decrease in time and will become zero when MAX_DO_JUMP_TIME is reached
			if (isDoingJump()) {

				if (m_StartJumpingTime == NOT_INITIALIZED_VALUE) {
					m_StartJumpingTime = Time::ms();
				}

				float doJumpTime = (curTime - m_StartJumpingTime) / 1000.0f;

				// don't get boost anymore
				if (doJumpTime > MAX_DO_JUMP_TIME) {
					doJumpTime = MAX_DO_JUMP_TIME;
					m_bJumped = true;
				}

				jumpVelocity += (1 - doJumpTime/MAX_DO_JUMP_TIME) * jumpVelocity * 0.3;
			}

			// player don't press key anymore
			else {
				m_bJumped = true;
			}

		} else {
			jumpingTime = (curTime - m_iStartFlyingTime) / 1000.0F;
		}

		m_vVelocity.y = jumpVelocity - m_fG * jumpingTime;

	} else {
		m_StartJumpingTime = NOT_INITIALIZED_VALUE;
	}

	// character don't move if both he wants to go to left and right
	float sideVelocity = 0.0f;
	if ( !((isDoingLeftMove() && isDoingRightMove())) ) {
		if (isDoingLeftMove())
			sideVelocity = -m_SideVelocity;
		if (isDoingRightMove())
			sideVelocity = m_SideVelocity;
	}

	if (fabs(sideVelocity) > 0.0f) {
		m_vVelocity.x = sideVelocity;
	} else {
		// avoid roughly stop moving when character is in air
		if (fabs(m_vVelocity.x) > 0.0f && !isGround()) {
			m_vVelocity.x *= AIR_FRICTION_FACTOR;
		} else {
			m_vVelocity.x = 0.0f;
		}
	}

	m_vOldPosition = m_vPosition;
	m_vPosition += m_vVelocity;
	m_HitBox.setPosition(m_vPosition);
}

void Character::setToGround() {
	m_vVelocity.y = -0.0001f;
	m_Jumping = false;
	m_bJumped = false;
	m_Falling = false;
	m_bFallen = false;
}

bool Character::isGround() {
	return !m_Jumping && !m_Falling;
}

void Character::hitCeiling() {
	m_vVelocity.y = -0.0001f;
	m_Jumping = false;
	m_bJumped = false;
	m_Falling = true;
	m_bFallen = false;
}

void Character::hitSide() {
	m_vVelocity.x = 0.0f;
}

void Character::fall() {
	m_Falling = true;
	m_bFallen = false; // fallen is false for initialize falling
}

bool Character::isFalling() {
	return m_Falling;
}

void Character::updateMisc() {

}

bool Character::isLookingLeft() const {
	return m_pWeaponSys->Facing().x < 0; // change facing in weaponDirection
}

void Character::setAlive() {
	m_State = ALIVE;
}
void Character::setDead() {
	m_State = DEAD;
}
bool Character::isDead() const {
	return m_State == DEAD;
}
bool Character::isAlive() const {
	return m_State == ALIVE;
}
bool Character::isSpawning() const {
	return m_State == SPAWNING;
}

void Character::Spawn() {

	const std::vector<Vector2D>& rSpawnPoints = m_pGameServer->GetMap()->SpawnPoints();
	int spawnPointIdx = RandInt(0, rSpawnPoints.size()-1);

	m_vPosition = rSpawnPoints[spawnPointIdx];
	m_HitBox.setPosition(m_vPosition);
	m_Health = m_MaxHealth;
	m_State = ALIVE;
}

bool Character::isJumping() const {
	return m_Jumping;
}

void Character::pain() const {

}

void Character::takeDamage(Projectile* pProj) {

	// avoid multiple kill messages
	if (m_State == DEAD)
		return;

	if (m_State == ALIVE) {
		m_Health -= pProj->Damage();
		GameServer::Event e;
		e.type = EVENT_PAIN;
		e.ownerClientId = m_ClientId;
		e.pos = pProj->Position();
		m_pGameServer->AddEvent(e);
	}

	if (m_Health <= 0) {
		m_KillerClientId = pProj->GetOwner()->m_ClientId;
		m_State = DEAD;
	}
}

void Character::Die() {

	GameServer::Event event;
	event.pos = m_vPosition - Vector2D(m_HitBox.Width(), m_HitBox.Height())/2;
	event.ownerClientId = m_ClientId;

	// normal death
	if (false && m_Health > -10 ) {
		event.type = EVENT_DEATH;
	} else {
		event.type = EVENT_GIB_DEATH;
	}

	m_pGameServer->AddEvent(event);

	m_State = SPAWNING;
	m_DeathTime = Time::ms();
}

void Character::onCollide(EntityType entType, OrientHV ori) {

}

bool Character::CanSpawn() const {
	return (Time::ms() - m_DeathTime) >= RESPAWNING_DELAY;
}

unsigned char Character::GetMoveState() const {
	unsigned char ms = MOVE_STATIONNARY;

	if (IsJetpacking()) {
		ms = MOVE_FLY;
	} else if ((m_Falling && !m_Jumping && !isEqual(m_vPosition.y, m_vOldPosition.y)) // avoid air walk
			|| m_Jumping) {
		ms = MOVE_JUMP;
	} else if (!isDoingLeftMove() && !isDoingRightMove()) {
		ms = MOVE_STATIONNARY;
	} else if ((isDoingLeftMove() && isLookingLeft())
			|| (isDoingRightMove() && !isLookingLeft())) {
		ms = MOVE_FORWARD;
	} else {
		ms = MOVE_BACKWARD;
	}

	return ms;
}

void Character::GetSnapshot(NetCharacter &netChar) {

	netChar.exist = true;// (m_State != INACTIVE);

	if (!netChar.exist)
		return;

	byte_t states = (m_State << 4) | GetMoveState();
	netChar.states = states;
	netChar.x = m_vPosition.x;
	netChar.y = m_vPosition.y;
	netChar.weaponType = m_pWeaponSys->CurrentWeapon()->getType();
	netChar.weaponAngle = WeaponSys()->WeaponAngle();
	netChar.ammo = WeaponSys()->CurrentWeapon()->AmmoCount();
	netChar.health = m_Health;
}

void Character::Init() {
	m_State = SPAWNING;
	std::memset(m_aInputs, 0, NUM_INPUTS);
	m_pWeaponSys->Reset();
}

void Character::Reset() {
	m_vPosition.Zero();
	m_vOldPosition.Zero();
	m_State = INACTIVE;
}

void Character::SetInputs(const unsigned char *pInput)
{
	std::memcpy(m_aInputs, pInput, NUM_INPUTS);
}

void Character::SetWeaponAngle(float weaponAngle) {
	m_pWeaponSys->SetWeaponAngle(weaponAngle);
}
