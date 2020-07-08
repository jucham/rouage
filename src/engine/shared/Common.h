#ifndef COMMON_H_
#define COMMON_H_

//#define CHEAT

enum
{
	MAX_CLIENTS = 8,
	INVALID_CLIENT_ID = -1,
	MAX_PROJECTILES = 256,
	MAX_EVENTS = 256,
	MAX_TRIGGERS = 128,
	MAX_WEAPON_ITEMS = 64,
	MAX_POWERUP_ITEMS = 64,
	MAX_PLAYERNAME_LENGTH = 32,
	RESPAWNING_DELAY = 1500
};

enum
{
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_JUMP,
	INPUT_JETPACK,
	INPUT_ATTACK,
	INPUT_PREV_WEAPON,
	INPUT_NEXT_WEAPON,
	INPUT_SHOW_SCOREBOARD, //TODO delete
	NUM_INPUTS
};

// Type of communication
enum
{
	NO_AUTH_COM = 0xFF, AUTH_COM = 0x01
};

// Type of data
enum
{
	NET_MSG, NET_INPUTS, NET_SNAPSHOT
};

/*
 Connection diagram - How the initilization works.

 Client -> INFO -> Server
 Contains version info, name, and some other info.

 Client <- MAP <- Server
 Contains current map.

 Client -> READY -> Server
 The client has loaded the map and is ready to go,
 but the mod needs to send it's information aswell.
 modc_connected is called on the client and
 mods_connected is called on the server.
 The client should call client_entergame when the
 mod has done it' s initilization.

 Client -> ENTERGAME -> Server
 Tells the server to start sending snapshots.
 client_entergame and server_client_enter is called.
 */

enum
{
	NETMSG_NULL = 0,

	// the first thing sent by the client
	// contains the version info for the client
	NETMSG_INFO,

	// sent by server
	NETMSG_CONNECTION_FAIL,
	NETMSG_MAPINFO,
	NETMSG_PLAYER_INFO,
	NETMSG_READY_TO_SPAWN,
	NETMSG_ENTER_IN_GAME,
	NETMSG_LEAVE,
	NETMSG_CHAT,
	NETMSG_CLIENT_DATA_CHANGE,
	NETMSG_EVENTS,

	NETMSG_MAP_CHANGE,		// sent when client should switch map
	NETMSG_MAP_DATA,			// map transfer, contains a chunk of the map file
	NETMSG_CON_READY,		// connection is ready, client should send start info
	NETMSG_SNAP,					// normal snapshot, multiple parts
	NETMSG_SNAPEMPTY,		// empty snapshot
	NETMSG_SNAPSINGLE,		
	NETMSG_SNAPSMALL,		
	NETMSG_INPUTTIMING,				// reports how off the input was
	NETMSG_RCON_AUTH_STATUS,	// result of the authentication
	NETMSG_RCON_LINE,				// line that should be printed to the remote console

	NETMSG_AUTH_CHALLANGE,	//
	NETMSG_AUTH_RESULT,		//

	// sent by client

	NETMSG_PROTOCOL_VERSION,

	NETMSG_READY,		
	NETMSG_ENTERGAME,
	NETMSG_INPUT,			// contains the inputdata from the client
	NETMSG_RCON_CMD,	
	NETMSG_RCON_AUTH,	
	NETMSG_REQUEST_MAP_DATA,

	NETMSG_AUTH_START,		
	NETMSG_AUTH_RESPONSE,

	// sent by both
	NETMSG_PING,
	NETMSG_PING_REPLY,
	NETMSG_ERROR,

	// sent by server
	NETMSG_RCON_CMD_ADD,
	NETMSG_RCON_CMD_REM,
};


enum {
	INVALID_EVENT,
	EVENT_FIRE_PISTOL,
	EVENT_FIRE_ROCKET_LAUNCHER,
	EVENT_FIRE_AK47,
	EVENT_FIRE_SHOTGUN,
	EVENT_NO_AMMO,
	EVENT_ROCKET_EXPLODE,
	EVENT_ITEM_PICKUP,
	EVENT_PAIN,
	EVENT_DEATH,
	EVENT_GIB_DEATH,
	EVENT_DESTROY_TILE,
	NUM_EVENTS
};

enum {
	CF_INVALID_PROTOCOL,
	CF_SERVER_FULL
};

#endif /* COMMON_H_ */
