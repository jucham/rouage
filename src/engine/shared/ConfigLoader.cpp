#include "ConfigLoader.h"
#include <cassert>
#include <base/tri_logger.hpp>
#include <cstring>

ConfigHandler::ConfigHandler(const char *pFileName) :
FileHandler(pFileName, false),
m_Properties()
{
	ReadData();
}

ConfigHandler::~ConfigHandler() {}

void ConfigHandler::ReadHeader() {}

void ConfigHandler::ReadData()
{

	if (m_LoadingSuccess)
	{
		const int MAX_LINE_LENGTH = 100;
		char aLine[MAX_LINE_LENGTH];
		std::string buf;

		while (1)
		{
			m_File.getline(aLine, MAX_LINE_LENGTH, '\n');

			int readCharacters = m_File.gcount();
			if (readCharacters < 3)
				break;

			char *pEqualChar = std::strchr(aLine, '=');

			if (!pEqualChar)
				continue;

			const int KEY_LENGTH = pEqualChar-aLine;
			char aKey[KEY_LENGTH+1];
			std::strncpy(aKey, aLine, KEY_LENGTH);
			aKey[KEY_LENGTH] = '\0';

			const int VALUE_LENGTH = (aLine+readCharacters) - pEqualChar + 1;
			char aValue[VALUE_LENGTH+1];
			std::strncpy(aValue, aLine+KEY_LENGTH+1, VALUE_LENGTH);
			aValue[VALUE_LENGTH] = '\0';

			std::string key(aKey);
			std::string value(aValue);

			m_Properties.insert(std::pair<std::string,std::string>(key,value));
		}
	}
}


void ConfigHandler::WriteData()
{
	std::ofstream file(m_Filename);

	if (file.is_open())
	{
		for (std::map<std::string,std::string>::iterator it=m_Properties.begin(); it!=m_Properties.end(); ++it)
		{
			std::string propertyName = it->first;
			std::string propertyValue = it->second;
			file << propertyName + "=" + propertyValue << std::endl;
		}
		file.close();
	}
}
