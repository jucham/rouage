#ifndef HEALTHGIVER_H
#define HEALTHGIVER_H

#include "PowerupGiver.h"

class TriggerSystem;

///////////////////////////////////////////////////////////////////////////////
///
/// \class HealthGiver HealthGiver.h
/// \brief This is a trigger that nurses a character
///
///////////////////////////////////////////////////////////////////////////////
class HealthGiver : public PowerupGiver
{
   public:

      /// \brief Constructor
      ///
      /// \param pTrigSys trigger system of health giver
      ///
      HealthGiver(GameServer *pGameServer, TriggerSystem* pTrigSys, Vector2D pos);

      void DoAction();


};

#endif // HEALTHGIVER_H
