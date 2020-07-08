#ifndef CLIENTCONFIG_H_
#define CLIENTCONFIG_H_

#include <string>
#include <engine/shared/Common.h>

struct ClientConfig {

	static unsigned int DefaultServerIp;
	static unsigned int DefaultServerPort;
	static char PlayerName[MAX_PLAYERNAME_LENGTH];
	static int SkinId;
	static int ScreenWidth;
	static int ScreenHeight;
	static int Fullscreen;

	static bool Load();
	static void Save();
	static bool SetDefaultServerIp(const std::string &address);
};

#endif /* CLIENTCONFIG_H_ */
