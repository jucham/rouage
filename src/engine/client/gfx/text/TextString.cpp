#include "TextString.h"

#include <string>
#include "../common/Timer.h"

TextString::TextString(const std::string& str,
                       unsigned int period,
                       bool transparency) :
   m_string (str), //( str.size(), TextChar()),
   m_iPeriod(0),
   m_iStartTimePeriod( Time::ms() ),
   m_bTransparency(false)
{

}



