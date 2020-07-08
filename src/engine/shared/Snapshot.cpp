#include "Snapshot.h"

NetField SnapshotData::charFields[] = {
		{NETF(exist, NetCharacter), NetField::BOOL},
		{NETF(states, NetCharacter), NetField::BYTE},
		{NETF(x, NetCharacter), NetField::FLOAT},
		{NETF(y, NetCharacter), NetField::FLOAT},
		{NETF(weaponType, NetCharacter), NetField::BYTE},
		{NETF(weaponAngle, NetCharacter), NetField::FLOAT},
		{NETF(ammo, NetCharacter), NetField::SHORT},
		{NETF(health, NetCharacter), NetField::BYTE}
};

NetField SnapshotData::playFields[] = {
		{NETF(kills, NetPlayer), NetField::SHORT},
		{NETF(deaths, NetPlayer), NetField::SHORT}
};


NetField SnapshotData::projFields[] = {
		{NETF(exist, NetProjectile), NetField::BOOL},
		{NETF(type, NetProjectile), NetField::BYTE},
		{NETF(x, NetProjectile), NetField::FLOAT},
		{NETF(y, NetProjectile), NetField::FLOAT},
		{NETF(angle, NetProjectile), NetField::FLOAT},
};

NetField SnapshotData::trigFields[] = {
		{NETF(exist, NetTrigger), NetField::BOOL},
		{NETF(type, NetTrigger), NetField::BYTE},
		{NETF(subType, NetTrigger),NetField::BYTE},
		{NETF(active, NetTrigger), NetField::BYTE},
		{NETF(x, NetTrigger), NetField::FLOAT},
		{NETF(y, NetTrigger), NetField::FLOAT}
};

int SnapshotData::numCharFields = NUM_FIELDS(charFields);
int SnapshotData::numPlayFields = NUM_FIELDS(playFields);
int SnapshotData::numProjFields = NUM_FIELDS(projFields);
int SnapshotData::numTrigFields = NUM_FIELDS(trigFields);
