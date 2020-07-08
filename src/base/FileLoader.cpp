#include "FileLoader.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

//------------------------------------
//-----------ctors&dtors--------------
//------------------------------------

FileHandler::FileHandler(const char *filename, bool exitOnLoadingFail) :
m_File(),
m_LoadingSuccess(true)
{
	std::strncpy(m_Filename, filename, MAX_FILENAME_SIZE);

	m_File.open(filename);

	if (!m_File.is_open()) {
		if (exitOnLoadingFail) {
			PrintError(filename);
			exit(0);
		} else {
			m_LoadingSuccess = false;
		}
	}
}

FileHandler::~FileHandler()
{
	m_File.close();
}


//------------------------------------
//-----------functions----------------
//------------------------------------

void FileHandler::ReadFile()
{
	if(m_File.is_open())
	{
		ReadHeader();
		ReadData();
	}
}

void FileHandler::PrintError(const char *pFilename) const {
	char errorStr[512];
	std::memset(errorStr, 0, 512);
	std::strcat(errorStr, "Fail to loading the file : ");
	std::strcat(errorStr, pFilename);
	printf("%s\n",errorStr);
}
