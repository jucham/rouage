#ifndef SENSORYMEMORY_H
#define SENSORYMEMORY_H

#include <map>
#include <list>

#include <base/Vector2D.h>

class Character;
class Bot;

///
/// \class MemoryRecord SensoryMemory.h
/// \brief this class lets recording of information about opponents
///
class MemoryRecord
{
   public:

      /// \brief records the time the opponent was last sensed (seen or heard). This
      /// is used to determine if a bot can 'remember' this record or not.
      /// (if CurrentTime() - m_dTimeLastSensed is greater than the bot's
      /// memory span, the data in this record is made unavailable to clients)
      int       iTimeLastSensed;

      /// \brief it can be useful to know how long an opponent has been visible. This
      /// variable is tagged with the current time whenever an opponent first becomes
      /// visible. It's then a simple matter to calculate how long the opponent has
      /// been in view (CurrentTime - iTimeBecameVisible)
      int       iTimeBecameVisible;

      /// \brief it can also be useful to know the last time an opponent was seen
      int       iTimeLastVisible;

      /// \brief a vector marking the position where the opponent was last sensed. This can
      /// be used to help hunt down an opponent if it goes out of view
      Vector2D    vLastSensedPosition;

      /// \brief set to true if opponent is within the field of view of the owner
      bool        bWithinFOV;

      /// \brief set to true if there is no obstruction between the opponent and the owner,
      /// permitting a shot.
      bool        bShootable;


      MemoryRecord() :
         iTimeLastSensed(-999),
         iTimeBecameVisible(-999),
         iTimeLastVisible(0),
         bWithinFOV(false),
         bShootable(false)
      {}
};

///
/// \class SensoryMemory SensoryMemory.h
/// \brief this class implements sensory memory that is an ensemble of questionable
/// memory records
///
class SensoryMemory
{
   public:

      SensoryMemory(Bot* pOwner, int MemorySpan);

      /// \brief this method is used to update the memory map whenever an opponent makes
      /// a noise
      void     UpdateWithSoundSource(Character* pNoiseMaker);

      /// \brief this removes a bot's record from memory
      void     RemoveBotFromMemory(Character* pChar);

      /// \brief this method iterates through all the opponents in the game world and
      /// updates the records of those that are in the owner's FOV
      void     UpdateVision();

      bool     isOpponentShootable(Character* pOpponent) const;
      bool     isOpponentWithinFOV(Character* pOpponent) const;
      Vector2D GetLastRecordedPositionOfOpponent(Character* pOpponent) const;
      int    GetTimeOpponentHasBeenVisible(Character* pOpponent) const;
      int    GetTimeSinceLastSensed(Character* pOpponent) const;
      int    GetTimeOpponentHasBeenOutOfView(Character* pOpponent) const;

      /// \brief this method returns a list of all the opponents that have had their
      /// records updated within the last m_iMemorySpan seconds.
      std::list<Character*> GetListOfRecentlySensedOpponents() const;

      void     RenderBoxesAroundRecentlySensed() const;

   private:

      typedef std::map<Character*, MemoryRecord> MemoryMap;

      Bot* m_pOwner; ///< the owner of this instance

      /// \brief this container is used to simulate memory of sensory events. A MemoryRecord
      /// is created for each opponent in the environment. Each record is updated
      /// whenever the opponent is encountered. (when it is seen or heard)
      MemoryMap  m_MemoryMap;

      /// \brief a bot has a memory span equivalent to this value. When a bot requests a
      /// list of all recently sensed opponents this value is used to determine if
      /// the bot is able to remember an opponent or not.
      int      m_iMemorySpan;

      /// \brief this methods checks to see if there is an existing record for pBot. If
      /// not a new MemoryRecord record is made and added to the memory map.(called
      /// by UpdateWithSoundSource & UpdateVision)
      void       MakeNewRecordIfNotAlreadyPresent(Character* pChar);

};

#endif // SENSORYMEMORY_H
