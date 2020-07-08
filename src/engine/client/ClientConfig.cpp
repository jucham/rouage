#include "ClientConfig.h"
#include <cstring>
#include <map>
#include <string>
#include <engine/shared/ConfigLoader.h>
#include <base/utils.h>
#include <base/tri_logger.hpp>
#include "Console.h"

unsigned int ClientConfig::DefaultServerIp = 127001;
unsigned int ClientConfig::DefaultServerPort = 30000;
char ClientConfig::PlayerName[MAX_PLAYERNAME_LENGTH];
int ClientConfig::SkinId = 0;
int ClientConfig::ScreenWidth = 800;
int ClientConfig::ScreenHeight = 600;
int ClientConfig::Fullscreen = false;

bool ClientConfig::Load()
{
	ConfigHandler configHandler("client.cfg");
	std::map<std::string,std::string> &properties = configHandler.GetProperties();

	if (properties.empty())
	{
		properties.insert(std::pair<std::string,std::string>("DefaultServerIp","127.0.0.1"));
		properties.insert(std::pair<std::string,std::string>("DefaultServerPort","30000"));
		properties.insert(std::pair<std::string,std::string>("PlayerName","Player"));
		properties.insert(std::pair<std::string,std::string>("SkinId","0"));
		properties.insert(std::pair<std::string,std::string>("ScreenWidth","800"));
		properties.insert(std::pair<std::string,std::string>("ScreenHeight","600"));
		properties.insert(std::pair<std::string,std::string>("Fullscreen","0"));

		printf("Client configuration loading failed, default configuration has been loaded\n");
	}

	// set the properties

	std::string defaultServerIp = properties["DefaultServerIp"];
	SetDefaultServerIp(defaultServerIp);

	int defaultServerPort = GetInt(properties["DefaultServerPort"]);
	if (true) //TODO check
		DefaultServerPort = defaultServerPort;

	std::string &playerName = properties["PlayerName"];
	if (true) //TODO check
		std::strncpy(PlayerName, playerName.c_str(), MAX_PLAYERNAME_LENGTH);

	int skinId = GetInt(properties["SkinId"]);
	if (true) //TODO check
		SkinId = skinId;
	

	int screenWidth = GetInt(properties["ScreenWidth"]);
	if (true) //TODO check
		ScreenWidth = screenWidth;

	int screenHeight = GetInt(properties["ScreenHeight"]);
	if (true) //TODO check
		ScreenHeight = screenHeight;

	int fullscreen = GetInt(properties["Fullscreen"]);
	if (true) //TODO check
		Fullscreen = fullscreen;

	//	Console::Instance().WriteLine("DefaultServerIp: " + properties["DefaultServerIp"]);
	//	Console::Instance().WriteLine("DefaultServerPort: " + properties["DefaultServerPort"]);
	//	Console::Instance().WriteLine("PlayerName: " + properties["PlayerName"]);
	//	Console::Instance().WriteLine("ScreenWidth: " + properties["ScreenWidth"]);
	//	Console::Instance().WriteLine("ScreenHeight: " + properties["ScreenHeight"]);
	//	Console::Instance().WriteLine("Fullscreen: " + properties["Fullscreen"]);

	return true;
}

void ClientConfig::Save()
{
	ConfigHandler configHandler("client.cfg");
	std::map<std::string,std::string> &properties = configHandler.GetProperties();

	int a = (DefaultServerIp >> 24);
	int b = (DefaultServerIp >> 16) & 0xff;
	int c = (DefaultServerIp >> 8) & 0xff;
	int d = DefaultServerIp & 0xff;

	char ip[16];
	std::sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
	properties["DefaultServerIp"] = ip;

	char str[256];
	std::sprintf(str, "%d", DefaultServerPort);
	properties["DefaultServerPort"] = str;

	properties["PlayerName"] = PlayerName;

	std::sprintf(str, "%d", SkinId);
	properties["SkinId"] = str;

	std::sprintf(str, "%d", ScreenWidth);
	properties["ScreenWidth"] = str;

	std::sprintf(str, "%d", ScreenHeight);
	properties["ScreenHeight"] = str;

	std::sprintf(str, "%d", Fullscreen);
	properties["Fullscreen"] = str;

	configHandler.WriteData();

	printf("client config saved\n");
}


bool ClientConfig::SetDefaultServerIp(const std::string &address) {
	int a, b, c, d;
	if(sscanf(address.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d)) {
		DefaultServerIp = (a << 24) | (b << 16) | (c << 8) | d;
		return true;
	}
	return false;
}
