#ifndef CHARACTER_H_INCLUDED
#define CHARACTER_H_INCLUDED

#include <string>
#include "MovingEntity.h"
#include <base/Time.h>
#include <engine/shared/Common.h>
#include <engine/server/ServerSnapshotManager.h>

class GameServer;
class WeaponSystem;
class Projectile;

enum State
{
	INACTIVE, SPAWNING, ALIVE, DEAD
};

enum MoveState
{
	MOVE_STATIONNARY, MOVE_FORWARD, MOVE_BACKWARD, MOVE_JUMP, MOVE_FLY, NUM_MOVE_STATES
};

///
/// \class Character Character.h
/// \brief This class implement a character, base of all character types
///
/// A character is a moving entity which is able to walk and jump, and possesses a
/// weapon (encapsulated in a weapon system) to attack its opponents.
///
class Character: public MovingEntity
{
	public:

		static const int NET_PACKET_SIZE = 15;

		static float m_fWIDTH;    ///< character width
		static float m_fHEIGHT;   ///< character height

		static float m_fARM_WIDTH;
		static float m_fARM_HEIGHT;

		/// \brief Constructor
		///
		/// \param pGame pointer to game
		/// \param name name of this character
		///
		Character(GameServer* pGame, int clientId);

		/// \brief Destructor
		///
		virtual ~Character();

		/// \fn void addLife(int n)
		/// \brief add life to character
		///
		/// \param n qty of life added
		///
		void addLife(int n);

		/// \fn  void removeLife(int n)
		/// \brief removes life to character
		///
		/// \param n qty of life removed
		///
		void removeLife(int n);

		/// \fn void renderInfoText()
		/// \brief Renders infos in text form indicates character state
		///
		/// Display lines of text, each line contains one info. Infos are : life,
		/// look direction, name, position, etc.
		///
		void renderInfoText();

		/// \fn void updatePos(Timer& timer)
		/// \brief Handles character moving
		///
		/// \param timer reference to absolute game time
		///
		/// This function watches character inputs (for a player inputs come from
		/// mouse and keyboard, for a bot inputs come from AI component), and do
		/// corresponding actions if they are allowed (example : a character
		/// won't jump if he's not ground). Physic computing is achieved to
		/// determine character's motion (gravity applying for jumping and falling by
		/// example).
		///
		void UpdatePos(); //TODO: kick argument timer

		/// \fn void setToGround()
		/// \brief Sets character as the ground
		///
		void setToGround();

		/// \fn bool isGround()
		/// \brief Asks if character is ground
		///
		/// @retval true if character is ground
		///
		bool isGround();

		/// \fn void hitCeiling()
		/// \brief Sets character as having hit the ceiling
		///
		/// Hit ceiling will result character's falling
		///
		void hitCeiling();

		/// \fn void hitSide()
		/// \brief Sets character as having hit one side (left or right)
		///
		/// Hit one side will put to zero character's x-velocity
		///
		void hitSide();

		/// \fn void fall()
		/// \brief Sets character as falling
		///
		void fall();

		/// \fn bool isFalling()
		/// \brief Asks if character is falling
		///
		/// \return true if character is falling
		///
		bool isFalling();

		/// \fn void updateMisc()
		/// \brief Updates miscelleanous thing
		///
		/// nothing yet
		///
		void updateMisc();

		/// \fn bool isLookingLeft()
		/// \brief Asks if character is looking left
		///
		/// \return true if character is looking left
		///
		bool isLookingLeft() const;

		/// \fn GameServer* GetGame()
		/// \brief Returns pointer to game
		///
		/// \return pointer to game
		///
		GameServer* GetGameServer()
		{
			return m_pGameServer;
		}

		/// \fn WeaponSystem* WeaponSys()
		/// \brief Returns pointer to weapon system of character
		///
		/// \return pointer to weapon system of character
		///
		WeaponSystem* WeaponSys()
		{
			return m_pWeaponSys;
		}

		/// \fn int Life() const
		/// \brief Returns qty of life of character
		///
		/// \return int number represents life of character
		///
		int GetLife() const
		{
			return m_Health;
		}

		int GetMaxLife() const;


		void Update();

		///
		/// \brief Updates all things of character (moving, weapons, etc.)
		///
		virtual void updateSpecial()=0;

		/// \fn virtual void render()
		/// \brief Renders character
		///
//      virtual void renderSpecial()=0;

		/// \fn bool isDoingLeftMove()
		/// \brief Asks if character has generated a left moving input
		///
		/// \return true if character has generated a left moving input
		///
		virtual bool isDoingLeftMove() const =0;

		/// \fn bool isDoingRightMove()
		/// \brief Asks if character has generated a right moving input
		///
		/// \return true if character has generated a right moving input
		///
		virtual bool isDoingRightMove() const =0;

		/// \fn bool isDoingJump()
		/// \brief Asks if character has generated a jumping input
		///
		/// \return true if character has generated a jumping input
		///
		virtual bool isDoingJump() = 0;

		virtual bool IsJetpacking() const = 0;

		void renderArm();
		void render();
		void setAlive();
		void setDead();
		bool isDead() const;
		bool isAlive() const;
		bool isSpawning() const;
		bool isInactive() const;
		void setSpawning();
		State GetState() const;
		void Spawn();
		bool isJumping() const;
		void pain() const;
		void takeDamage(Projectile* pProj);
		void Die();
		void onCollide(EntityType entType, OrientHV ori);
		bool CanSpawn() const;
		std::string Name() const;
		unsigned char GetMoveState() const;
		void GetSnapshot(NetCharacter &netChar);

		void setClientId(int clientId)
		{
			m_ClientId = clientId;
		}

		void SetInputs(const unsigned char *pInput);
		void SetWeaponAngle(float weaponAngle);

		void Init();
		void Reset();

		unsigned char GetInput(int inputType) const;

		int GetClientId() const;
		int GetKillerClientId() const;

	protected:

		enum
		{
			FOOTSTEP_HIGH, FOOTSTEP_LOW
		};

		static const int NOT_INITIALIZED_VALUE = -1;
		static const float MAX_DO_JUMP_TIME;
		static const float AIR_FRICTION_FACTOR;

		State m_State;
		MoveState m_MoveState;
		int m_Health;          ///< qty of life of character
		int m_MaxHealth;
		bool m_Jumping;            ///< indicates if character is jumping
		bool m_bJumped;             ///< indicates if character already jumped, useful for initializing
		bool m_Falling;            ///< indicates if character is falling
		bool m_bFallen;             ///< indicates if character already fallen, useful for initializing
		float m_SideVelocity;      ///< magnitude of side moving force
		float m_InitJumpVelocity;   ///< magnitude of jumping impulse force
		float m_JetpackVelocity;
		WeaponSystem  *m_pWeaponSys;       ///< Weapon system of character
		int m_DeathTime;
		int m_ClientId;
		int m_KillerClientId;
		unsigned char m_aInputs[NUM_INPUTS];
		bool m_JumpReleased;
		int m_StartJumpingTime;
		bool m_OwnJetpack;
		bool m_Jetpacking;
		int m_StartJetpackingTime;
		float m_JetpackFallVelocity;
};

inline State Character::GetState() const
{
	return m_State;
}

inline bool Character::isInactive() const
{
	return (m_State == INACTIVE);
}

inline void Character::setSpawning()
{
	m_State = SPAWNING;
}

inline unsigned char Character::GetInput(int inputType) const
{
	assert(inputType >= 0 && inputType < NUM_INPUTS);
	return m_aInputs[inputType];
}

inline int Character::GetClientId() const {return m_ClientId;}
inline int Character::GetKillerClientId() const {return m_KillerClientId;}

#endif // CHARACTER_H_INCLUDED
