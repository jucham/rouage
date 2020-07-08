#ifndef TARGETINGSYSTEM_H
#define TARGETINGSYSTEM_H

#include <list>
#include <base/Vector2D.h>

class Character;
class Bot;

///
/// \class TargetingSystem TargetingSystem.h
/// \brief This class implements bot targeting system
///
class TargetingSystem
{
   public:

      TargetingSystem(Bot* pOwner);

      /// \brief updates targeting system
      ///
      /// each time this method is called the opponents in the owner's sensory
      /// memory are examined and the closest  is assigned to m_pCurrentTarget.
      /// if there are no opponents that have had their memory records updated
      /// within the memory span of the owner then the current target is set
      /// to null
      void       Update();

      /// \brief returns true if there is a currently assigned target
      bool       isTargetPresent() const;

      /// \brief returns true if the target is within the field of view of the owner
      bool       isTargetWithinFOV() const;

      /// \brief returns true if there is unobstructed line of sight between the target
      /// and the owner
      bool       isTargetShootable() const;

      /// \brief returns the position the target was last seen. Throws an exception if
      /// there is no target currently assigned
      Vector2D   GetLastRecordedPosition() const;

      /// \brief returns the amount of time the target has been in the field of view
      int      GetTimeTargetHasBeenVisible() const;

      /// \brief returns the amount of time the target has been out of view
      int      GetTimeTargetHasBeenOutOfView() const;

      /// \brief returns a pointer to the target. null if no target current.
      Character* GetTarget() const;

      /// \brief sets the target pointer to null
      void       ClearTarget();

  private:

      Bot*        m_pOwner;         ///< owner of that system
      Character*  m_pCurrentTarget; ///< the current target (this will be null if there is no target assigned)
};


inline bool TargetingSystem::isTargetPresent() const { return m_pCurrentTarget != 0; }

inline Character* TargetingSystem::GetTarget() const { return m_pCurrentTarget; }

inline void TargetingSystem::ClearTarget() { m_pCurrentTarget=0; }

#endif // TARGETINGSYSTEM_H
