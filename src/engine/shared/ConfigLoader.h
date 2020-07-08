#ifndef CONFIGLOADER_H_
#define CONFIGLOADER_H_

#include <map>
#include <base/FileLoader.h>

class ConfigHandler : public FileHandler
{
public:
	ConfigHandler(const char *pFileName);
	~ConfigHandler();
	void ReadHeader();
	void ReadData();
	void WriteData();
	std::map<std::string, std::string> & GetProperties() {return m_Properties;}

private:
	std::map<std::string, std::string> m_Properties;
};

#endif /* CONFIGLOADER_H_ */
