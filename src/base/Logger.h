#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <fstream>

class Logger
{
   public:
      static void write(const std::string& filename, const std::string& str);
};

#endif // LOGGER_H
