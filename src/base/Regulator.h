#ifndef REGULATOR_H
#define REGULATOR_H

#include <base/Time.h>

enum { MS_VALUE, HERTZ_VALUE };

class Regulator
{
   public:

      Regulator(int val, unsigned valType);

      bool isReady();

   protected:
      int m_iTimeBetweenUpdates;
      int m_iNextTimeUpdate;
};

inline bool Regulator::isReady()
{
   int curTime = Time::ms();
   if ( curTime >= m_iNextTimeUpdate )
   {
      m_iNextTimeUpdate = curTime + m_iTimeBetweenUpdates;
      return true;
   }

   return false;
}

#endif // REGULATOR_H
