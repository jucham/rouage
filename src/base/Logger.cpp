#include "Logger.h"

void Logger::write(const std::string& filename, const std::string& str)
{
   std::ofstream file(filename.c_str(), std::ofstream::app);
   file << str;
   file.close();
}
