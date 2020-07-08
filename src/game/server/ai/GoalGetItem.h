#ifndef GOALGETITEM_H
#define GOALGETITEM_H

#include "GoalComposite.h"
#include "GoalTypes.h"

class Bot;
class Trigger;

/// \brief helper function to change an item type enumeration into a goal type
int ItemTypeToGoalType(int itemType, int type);


///
/// \class GoalGetItem GoalGetItem.h
/// \brief a composite goal to grab a given item
///
class GoalGetItem : public GoalComposite<Bot>
{
   public:

     GoalGetItem(Bot* pBot, int itemType, int type);

     void Activate();
     int  Process();
     void Terminate();

   private:

      /// \brief true if item is inactive
      bool hasItemBeenStolen() const;

      int               m_iItemType;      ///< type of item, i.e. powerups or weapon
      int               m_iType;          ///< type for a given item type
      const Trigger*    m_pGiverTrigger;  ///< trigger that gives an item

};


#endif // GOALGETITEM_H
