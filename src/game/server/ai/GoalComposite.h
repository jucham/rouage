#ifndef GOALCOMPOSITE_H_INCLUDED
#define GOALCOMPOSITE_H_INCLUDED

#include <list>
#include "Goal.h"

#include <base/debug.h>

///
/// \class GoalComposite GoalComposite.h
/// \brief This class implements a composite goal based on Goal class
///
/// A composite goal contains one or severals subgoals. When all these subgoals are
/// processed then composite goal is done
///
template <class entity_type>
class GoalComposite : public Goal<entity_type>
{
   public:

      /// \brief Constructs a composite goal for an entity \a pE with goal type \a type
      GoalComposite(entity_type* pE, int type);

      /// \brief Destructs a composite goal, that means removes all subgoals
      ///
      /// when this object is destroyed make sure any subgoals are terminated
      /// and destroyed.
      virtual ~GoalComposite();

      /// \brief logic to run when the goal is activated.
      virtual void Activate()=0;

      /// \brief logic to run each update-step.
      virtual int  Process()=0;

      /// \brief logic to run prior to the goal's destruction
      virtual void Terminate()=0;

      /// \brief adds a subgoal to the front of the subgoal list
      void AddSubgoal(Goal<entity_type>* g);

      /// \brief removes all subgoal that contains subgoal list.
      ///
      /// this method iterates through the subgoals and calls each one's Terminate
      /// method before deleting the subgoal and removing it from the subgoal list
      void RemoveAllSubgoals();

   protected:

      typedef std::list<Goal<entity_type>* > SubgoalList;

      SubgoalList   m_SubGoals; ///< subgoals that contains a composite goal

      /// \brief processes any subgoals that may be present
      int  ProcessSubgoals();

};


template <class entity_type>
GoalComposite<entity_type>::GoalComposite(entity_type* pE, int type) : Goal<entity_type>(pE,type) {}


template <class entity_type>
GoalComposite<entity_type>::~GoalComposite() { RemoveAllSubgoals(); }


template <class entity_type>
void GoalComposite<entity_type>::AddSubgoal(Goal<entity_type>* g)
{
   //add the new goal to the front of the list
   m_SubGoals.push_front(g);
}

template <class entity_type>
void GoalComposite<entity_type>::RemoveAllSubgoals()
{
   for (typename SubgoalList::iterator it = m_SubGoals.begin();
         it != m_SubGoals.end();
         ++it)
   {
      (*it)->Terminate();
      delete *it;
   }

  m_SubGoals.clear();
}


//  this method first removes any completed goals from the front of the
//  subgoal list. It then processes the next goal in the list (if there is one)
//
template <class entity_type>
int GoalComposite<entity_type>::ProcessSubgoals()
{
   //remove all completed and failed goals from the front of the subgoal list
   while (!m_SubGoals.empty()
            && (m_SubGoals.front()->isComplete() || m_SubGoals.front()->hasFailed()))
   {
      m_SubGoals.front()->Terminate();
      delete m_SubGoals.front();
      m_SubGoals.pop_front();
   }

   //if any subgoals remain, process the one at the front of the list
   if (!m_SubGoals.empty())
   {
      //grab the status of the front-most subgoal
      int StatusOfSubGoals = m_SubGoals.front()->Process();

      //we have to test for the special case where the front-most subgoal
      //reports 'completed' *and* the subgoal list contains additional goals.When
      //this is the case, to ensure the parent keeps processing its subgoal list
      //we must return the 'active' status.
      if (StatusOfSubGoals == Goal<entity_type>::completed && m_SubGoals.size() > 1)
      {
         return Goal<entity_type>::active;
      }

      return StatusOfSubGoals;
   }
   //no more subgoals to process - return 'completed'
   else
   {
      return Goal<entity_type>::completed;
   }
}


#endif // GOALCOMPOSITE_H_INCLUDED
