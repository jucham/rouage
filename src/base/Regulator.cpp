#include "Regulator.h"
#include <cassert>

Regulator::Regulator(int val, unsigned valType) :
   m_iTimeBetweenUpdates(0),
   m_iNextTimeUpdate(0)
{
   switch(valType)
   {
      case MS_VALUE:
      {
         m_iTimeBetweenUpdates = val;
         break;
      }

      case HERTZ_VALUE:
      {
         m_iTimeBetweenUpdates = static_cast<int> (1000.0f / val);
         break;
      }

      default:
         assert (0 && "Bad timer value type");
         break;
   }
}
