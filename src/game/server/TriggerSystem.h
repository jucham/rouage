#ifndef TRIGGERSYSTEM_H
#define TRIGGERSYSTEM_H

#include <list>
#include "Trigger.h"


class Map;
class Trigger;
class NetTrigger;

class TriggerSystem
{
   public:
      typedef std::list<Trigger*> TriggerPtrsList;

      TriggerSystem(Map &map);
      ~TriggerSystem();

      void AddTrigger(Trigger* t);
      void UpdateTriggers();
      void GetSnapshot(NetTrigger *netTrigs, byte_t *skip);
      Map & GetMap() {return m_Map;}

   protected:
      Map &m_Map;
      TriggerPtrsList   m_TriggerPtrs;

};

#endif // TRIGGERSYSTEM_H
