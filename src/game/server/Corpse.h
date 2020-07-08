#ifndef CORPSE_H
#define CORPSE_H

#include "MovingEntity.h"


class Corpse
{
   public:
      Corpse(GameServer* pGame, Vector2D pos, OrientTBLR ori, bool isPlayer);

      void onCollide(EntityType entType, OrientHV ori);
      void update();
      void prepareRender(float* varray, unsigned int arrayPos);
     bool isPlayer() const;

   protected:
      void updateSpecial();

      bool     m_bIsPlayer;
      bool     m_bIsGibbed;
      int      m_iStartRotTime;
      int      m_iRotTimeLength;
      float    m_fRotAngle;
      float    m_iTranspaPeriod; ///< that value indicates on how many time transparency is active

};

inline bool Corpse::isPlayer() const {return m_bIsPlayer;}

#endif // CORPSE_H
