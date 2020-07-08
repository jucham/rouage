#include "Weapon.h"
#include <cassert>
#include <base/Time.h>
#include <base/debug.h>
#include "WeaponSystem.h"
#include "GameServer.h"

Dimension Weapon::m_fWeaponDims[NUM_WEAPONS];

Weapon::Weapon(WeaponSystem* pws, int type) :
      m_pWeaponSys(pws),
      m_fWidth(m_fWeaponDims[type].width),
      m_fHeight(m_fWeaponDims[type].height),
      m_iType(type),
      m_TimeNextAvailable(0),
      m_DelayBetweenShots(0),
      m_AmmoCount(0),
      m_MaxAmmoCount(0),
      m_fHandOffset(0.0f),
      m_fProjCannonOffset(0.0f)
{
   assert(pws != 0);
}

Weapon::~Weapon()
{
}

void Weapon::Reset()
{
	m_AmmoCount = m_MaxAmmoCount;
}

bool Weapon::Ready()
{
   return Time::ms() >= m_TimeNextAvailable;
}

void Weapon::ResetTimeDelay() {
	m_TimeNextAvailable = Time::ms() + m_DelayBetweenShots;
}

void Weapon::DoFire()
{
	Fire();
	m_AmmoCount -= 1;

#if defined(CHEAT)
	m_AmmoCount++;
#endif

}

int Weapon::AmmoCount() const
{
   return m_AmmoCount;
}

bool Weapon::NoAmmo() const
{
   return m_AmmoCount == 0;
}

int Weapon::MaxAmmoCount() const
{
   return m_MaxAmmoCount;
}

void Weapon::getFullAmmo()
{
   m_AmmoCount = m_MaxAmmoCount;
}

int Weapon::getType() const
{
   return m_iType;
}

float Weapon::HandOffset() const
{
   return m_fHandOffset;
}

