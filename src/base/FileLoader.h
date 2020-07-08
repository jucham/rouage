#ifndef FILELOADER_H
#define FILELOADER_H

#include <iostream>
#include <fstream>

class FileHandler
{
public:
	FileHandler(const char *pFileName, bool exitOnLoadingFail = true); // throw(std::ifstream::failure);
	virtual ~FileHandler();
	void ReadFile();
	virtual void ReadHeader()=0;
	virtual void ReadData()=0;

protected:
	void PrintError(const char *pFileName) const;

	static const int MAX_FILENAME_SIZE = 256;
	char m_Filename[MAX_FILENAME_SIZE];
	std::ifstream    m_File;
	bool m_LoadingSuccess;

};

#endif // FILELOADER_H
