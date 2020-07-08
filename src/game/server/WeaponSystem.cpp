//TODO: COMMENT
#include "WeaponSystem.h"
#include <base/utils.h>
#include "Weapon.h"
#include "Pistol.h"
#include "RocketLauncher.h"
#include "AK47.h"
#include "Shotgun.h"
#include "GameServer.h"
#include "Character.h"
#include "EntityEnumerations.h"

WeaponSystem::WeaponSystem(GameServer* pGameServer) :
m_pGame(pGameServer),
m_Weapons(NUM_WEAPONS),
m_Direction(),
m_vPosition(),
m_fWeaponAngle(0)
{
	assert ( pGameServer != 0 );

	CreateWeapons();
	Reset();
}

WeaponSystem::~WeaponSystem()
{}

void WeaponSystem::CreateWeapons() {
	m_Weapons[WP_PISTOL] = new Pistol(this);
	m_Weapons[WP_ROCKET_LAUNCHER] = new RocketLauncher(this);
	m_Weapons[WP_AK47] = new AK47(this);
	m_Weapons[WP_SHOTGUN] = new Shotgun(this);
}

void WeaponSystem::Reset()
{
#if defined(CHEAT)
	PossessAllWeapons();
#else
	EmptyInventory();
#endif

	ResetAllWeapons();
	PickWeapon(WP_PISTOL);
	SelectWeapon(WP_PISTOL);
}

void WeaponSystem::EmptyInventory() {
	std::memset(m_PossessWeapon, 0, sizeof(m_PossessWeapon[0]) * NUM_WEAPONS);
}

void WeaponSystem::PossessAllWeapons() {
	std::memset(m_PossessWeapon, 1, sizeof(m_PossessWeapon[0]) * NUM_WEAPONS);
}

void WeaponSystem::ResetAllWeapons() {
	for (int i = 0; i < NUM_WEAPONS; ++i) {
		m_Weapons[i]->Reset();
	}
}

void WeaponSystem::PickWeapon(int weaponType) {
	m_PossessWeapon[weaponType] = true;
}

Weapon* WeaponSystem::getWeaponFromInventory(int weaponType)
{
	return m_Weapons[weaponType];
}

void WeaponSystem::computeWeaponAngle()
{
	m_fWeaponAngle = atan2f(m_Direction.y, m_Direction.x);
}


void WeaponSystem::SelectWeapon(const int weaponType)
{
	assert (weaponType >= 0 && weaponType < NUM_WEAPONS);
	m_pCurrentWeapon = m_Weapons[weaponType];
}

//TODO factoriser ça
void WeaponSystem::selectPrevWeapon()
{
	int prevWeaponType = (m_pCurrentWeapon->getType()-1) % NUM_WEAPONS;
	prevWeaponType = (prevWeaponType == -1) ? NUM_WEAPONS-1 : prevWeaponType;

	while ( ! m_PossessWeapon[prevWeaponType] )
	{
		prevWeaponType = (prevWeaponType-1) % NUM_WEAPONS;
		prevWeaponType = (prevWeaponType == -1) ? NUM_WEAPONS-1 : prevWeaponType;
	}

	SelectWeapon( prevWeaponType );
}

void WeaponSystem::selectNextWeapon()
{
	int nextWeaponType = (m_pCurrentWeapon->getType()+1) % NUM_WEAPONS;

	while ( ! m_PossessWeapon[nextWeaponType] )
	{
		nextWeaponType = (nextWeaponType+1) % NUM_WEAPONS;
	}

	SelectWeapon( nextWeaponType );
}

bool WeaponSystem::PossessWeapon(const int weaponType) const
{
	return m_PossessWeapon[weaponType];
}

