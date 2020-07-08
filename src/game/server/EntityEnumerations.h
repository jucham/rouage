#ifndef ENTITYENUMERATIONS_H_INCLUDED
#define ENTITYENUMERATIONS_H_INCLUDED

enum EntityType
{
   ENTITY_TILE,
   ENTITY_TRIGGER,
   ENTITY_PROJECTILE,
   ENTITY_CHARACTER,
   ENTITY_NONE,
   ENTITY_TYPE_NUM
};

// Items
enum
{
   IT_POWERUP,
   IT_WEAPON,
   NUM_ITEMS
};

// PowerUps
enum
{
   PW_HEALTH,
   NUM_POWERUPS
};

// Trigger
enum
{
	TG_ITEM_POWERUP,
	TG_ITEM_WEAPON,
	NUM_TRIGGERS
};

// Weapon
enum
{
   WP_PISTOL,
   WP_ROCKET_LAUNCHER,
   WP_SHOTGUN,
   WP_AK47,
   NUM_WEAPONS
};

// Projectiles
enum
{
   PJ_BULLET,
   PJ_ROCKET,
   NUM_PROJECTILES
};


#endif // ENTITYENUMERATIONS_H_INCLUDED
