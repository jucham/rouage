#ifndef TEXTSTRING_H
#define TEXTSTRING_H

#include <list>
#include <string>

#include <base/Time.h>
#include <base/Vector2D.h>

class TextString
{
   public:
      TextString(const std::string& str=std::string(""),
                 unsigned int period=0,
                 bool transparency=false);

      bool isPermanent() const;
      bool renderTimeElapsed() const;

      const std::string& getString() const;
      const Vector2D& getPos() const;

   protected:
      std::string    m_string;
      Vector2D       m_pos;
      unsigned int   m_iPeriod;
      unsigned int   m_iStartTimePeriod;
      bool           m_bTransparency;

};

inline bool TextString::isPermanent() const
{
   return  m_iPeriod == 0;
}

inline bool TextString::renderTimeElapsed() const
{
   return  Time::ms() - m_iStartTimePeriod > m_iPeriod;
}

inline const std::string& TextString::getString() const
{
   return m_string;
}

inline const Vector2D& TextString::getPos() const
{
   return m_pos;
}

#endif // TEXTSTRING_H
