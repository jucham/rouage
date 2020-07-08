#ifndef WEAPONSYSTEM_H
#define WEAPONSYSTEM_H

#include <vector>
#include <base/Vector2D.h>
#include "EntityEnumerations.h"

class GameServer;
class Weapon;
class Character;

class WeaponSystem
{
protected:


	GameServer *m_pGame;
	std::vector<Weapon*>  m_Weapons;
	Weapon     *m_pCurrentWeapon;
	Vector2D   m_Direction;
	Vector2D   m_vPosition;
	float      m_fWeaponAngle;
	bool       m_PossessWeapon[NUM_WEAPONS];

public:
	WeaponSystem(GameServer* pGame);
	virtual ~WeaponSystem();

	void Reset();

	Weapon* getWeaponFromInventory(int weaponType);

	virtual void Update()=0;

	void renderCurrentWeapon();
	void computeWeaponAngle();
	void SelectWeapon(const int weaponType);

	void selectPrevWeapon();
	void selectNextWeapon();

	void PickWeapon(int weaponType);
	bool PossessWeapon(const int weaponType) const;


	GameServer* GetGameServer() {return m_pGame;}

	virtual Character* Shooter() const = 0;

	const Vector2D& Facing() const {return m_Direction;}
	void  SetFacing(Vector2D facing) { m_Direction = facing; }
	const Weapon* CurrentWeapon() const {return m_pCurrentWeapon;}
	Weapon* CurrentWeapon() {return m_pCurrentWeapon;}
	const Vector2D& WeaponPos() const {return m_vPosition;}
	float WeaponAngle() const {return m_fWeaponAngle;}
	void SetWeaponAngle(float weaponAngle) {m_fWeaponAngle = weaponAngle;}

	virtual bool ChangeWeapon()=0;
	virtual void UpdateDirection()=0;
	virtual bool IsShooting()=0;

private:
	void CreateWeapons();
	void ResetAllWeapons();
	void EmptyInventory();
	void PossessAllWeapons();
};

#endif // WEAPONSYSTEM_H
