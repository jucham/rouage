#include "CollisionManager.h"

#include "CollisionModel.h"
#include <game/server/GameServer.h>
#include "Character.h"
#include "Map.h"
#include "CharacterCollisionModels.h"
#include <game/ProjectileCollisionModels.h>


CollisionManager::CollisionManager(GameServer* pGameServer) :
   m_pGameServer(pGameServer),

   m_pColModCharTrig(new CollisionModelCharacterTrigger( pGameServer->Characters(), pGameServer->GetMap()->getTiles() )),
   m_pColModCharProj(new CollisionModelCharacterProjectile( pGameServer->Characters(), pGameServer->GetMap()->getTiles() )),
   m_pColModCharTile(new CollisionModelCharacterTile( pGameServer->Characters(), pGameServer->GetMap()->getTiles() )),
   m_pColModProjTile(new CollisionModelProjectileTile( pGameServer->Projectiles(), pGameServer->GetMap()->getTiles()))
{}

CollisionManager::~CollisionManager()
{
   delete m_pColModCharTrig;
   delete m_pColModCharProj;
   delete m_pColModCharTile;
   delete m_pColModProjTile;
}

void CollisionManager::collideCharacters()
{
   // Character versus Trigger
   m_pColModCharTrig->CollideEntities();

   // Character versus Projectile
   m_pColModCharProj->CollideEntities();

   // Character versus Tile
   m_pColModCharTile->CollideEntities();
}

void CollisionManager::collideProjectiles()
{
   // Projectile versus Tile
   m_pColModProjTile->CollideEntities();
}
