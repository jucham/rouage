//TODO: COMMENT

#include "Projectile.h"
#include <vector>
#include <base/Time.h>
#include <base/debug.h>
#include "Character.h"
#include "GameServer.h"
#include "Weapon.h"
#include "WeaponSystem.h"
#include "Tile.h"
#include "Map.h"
#include <engine/server/ServerSnapshotManager.h>

bool Projectile::m_UsedArrayIds[MAX_PROJECTILES];

Projectile::Projectile(Character* pShooter, const Vector2D& facing) :
		MovingEntity(pShooter->GetGameServer(), Time::ms(), Vector2D(), Tile::m_fHEIGHT * 0.8f),
		m_ArrayId(GetNextFreeArrayId()),
		m_Exist(true),
		m_WeaponType(0),
		m_pOwner(pShooter),
		m_iDamage(0),
		m_bDead(false),
		m_vImpulseForce(facing),
		m_fMagnitudeFactor(0.0f),
		m_pOldTileSpot(0),
		m_pCurrentTileSpot(0)
{
	assert(pShooter != 0);

	WeaponSystem* pWs = m_pOwner->WeaponSys();
	// create vector to align projectile with cannon
	Vector2D cannonAlign(facing.Perp());
	cannonAlign *= pWs->CurrentWeapon()->m_fProjCannonOffset;
	cannonAlign *= (m_pOwner->isLookingLeft()) ? -1.0f : +1.0f;

	m_vPosition = pWs->WeaponPos() + pWs->Facing() * (pWs->CurrentWeapon()->HandOffset() + pWs->CurrentWeapon()->Width() / 2) + cannonAlign;
	m_HitBox.setPosition(m_vPosition);
}

Projectile::~Projectile()
{
	m_UsedArrayIds[m_ArrayId] = false;
}

void Projectile::setImpulseForce()
{
	// Calculate magnitude according to character width
	// move of a projectile should not be more than character width
	// if it was the case then projectile may go through character
	float impulseForceMagnitude = Character::m_fWIDTH * m_fMagnitudeFactor;
	m_vImpulseForce *= impulseForceMagnitude;
}

void Projectile::update()
{
	// Updates time
	int curTime = Time::ms();
	int throwingTime = curTime - m_iStartFlyingTime;

	// Computes velocity
	m_vVelocity.x = m_vImpulseForce.x;
	m_vVelocity.y = m_vImpulseForce.y - (m_fG * throwingTime / 1000.0f);

	m_vOldPos2.x = m_vOldPosition.x;
	m_vOldPos2.y = m_vOldPosition.y;

	m_vOldPosition.x = m_vPosition.x;
	m_vOldPosition.y = m_vPosition.y;

	m_vPosition += m_vVelocity;
	m_HitBox.setPosition(m_vPosition);

	updateSpecial();

	// let projectiles non-collided to die after 3 seconds
	if (throwingTime > 3000)
	{
		m_bDead = true;
	}

	// Updates where is projectile
	updateTileSpot();
}

void Projectile::updateTileSpot()
{
	if (m_bDead)
	{
		removeTileSpot();
	}
	else
	{
		m_pOldTileSpot = m_pCurrentTileSpot;
		m_pCurrentTileSpot = CurrentTileSpot();

		if (m_pCurrentTileSpot != m_pOldTileSpot)
		{
			m_pOldTileSpot->removeProjectile(ID());
			m_pCurrentTileSpot->addProjectile(this);
		}
	}
}

void Projectile::removeTileSpot()
{
	if (m_pOldTileSpot)
		m_pOldTileSpot->removeProjectile(ID());
	if(m_pCurrentTileSpot)
		m_pCurrentTileSpot->removeProjectile(ID());
}

Tile* Projectile::CurrentTileSpot()
{
	Map::TilePtr2dVector& rTiles = m_pOwner->GetGameServer()->GetMap()->getTiles();

	int col = static_cast<int>(std::floor((m_HitBox.Position().x + m_HitBox.Width() / 2) / Tile::m_fWIDTH));
	int lin = static_cast<int>(std::floor((m_HitBox.Position().y + m_HitBox.Height() / 2) / Tile::m_fHEIGHT));

	// checks indexes
	int maxCol = static_cast<int>(rTiles.size()) - 1;
	int maxLin = static_cast<int>(rTiles[0].size()) - 1;
	if (col < 0 || lin < 0 || col > maxCol || lin > maxLin)
		return m_pCurrentTileSpot;

	return rTiles[col][lin];
}

bool Projectile::wasShotFrom(int ms)
{
	return (Time::ms() - m_iStartFlyingTime) > ms;
}

void Projectile::GetSnapshot(NetProjectile *netProjs)
{
	NetProjectile &np = netProjs[m_ArrayId];
	m_Exist = (isDead()) ? false : true;
	np.exist = m_Exist;
	np.type = m_WeaponType;
	np.x = m_vPosition.x;
	np.y = m_vPosition.y;
	np.angle = GetAngle();
}

