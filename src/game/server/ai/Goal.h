#ifndef GOAL_H_INCLUDED
#define GOAL_H_INCLUDED

#include <stdexcept>
#include <typeinfo>

#include <base/Vector2D.h>
#include <game/server/GameServer.h>
#include "GoalTypes.h"

///
/// \class Goal Goal.h
/// \brief This class implements atomic goal,the base for goal oriented bot
///
/// that class represents an atomic goal, which means that goal is a elementary
/// action (like move to a point to an other point)
///
template <class entity_type>
class Goal
{
   public:

      enum {active, inactive, completed, failed};

      Goal(entity_type*  pE, int type);
      virtual ~Goal();

      /// \brief logic to run when the goal is activated.
      virtual void Activate()=0;

      /// \brief logic to run each update-step
      virtual int  Process()=0;

      /// \brief logic to run when the goal is satisfied. (typically used to switch
      /// off any things)
      virtual void Terminate()=0;

      /// \brief A Goal is atomic and cannot aggregate subgoals yet we must implement
      /// this method to provide the uniform interface required for the goal
      /// hierarchy.
      virtual void AddSubgoal(Goal<entity_type>* g);

      bool isComplete() const;
      bool isActive() const;
      bool isInactive() const;
      bool hasFailed() const;

      /// \brief returns type of goal
      int  GetType() const;

   protected:

      /// \brief if status is inactive this method sets it to active and calls Activate()
      void ActivateIfInactive();

      /// \brief if status is failed this method sets it to inactive so that the goal
      /// will be reactivated (and therefore re-planned) on the next update-step.
      void ReactivateIfFailed();

      int             m_iType;    ///< an enumerated type specifying the type of goal
      entity_type*    m_pOwner;   ///< a pointer to the entity that owns this goal
      int             m_iStatus;  ///< an enumerated value indicating the goal's status (active, inactive, completed, failed)




};


template <class entity_type>
Goal<entity_type>::Goal(entity_type*  pE, int type) :
   m_iType(type),
   m_pOwner(pE),
   m_iStatus(inactive)
{

}

template <class entity_type>
Goal<entity_type>::~Goal() {}


template <class entity_type>
void Goal<entity_type>::AddSubgoal(Goal<entity_type>* g)
{
   throw std::runtime_error("Cannot add goals to atomic goals");
}


template <class entity_type>
bool Goal<entity_type>::isComplete() const {return m_iStatus == completed;}

template <class entity_type>
bool Goal<entity_type>::isActive() const {return m_iStatus == active;}

template <class entity_type>
bool Goal<entity_type>::isInactive() const{return m_iStatus == inactive;}

template <class entity_type>
bool Goal<entity_type>::hasFailed() const{return m_iStatus == failed;}

template <class entity_type>
int Goal<entity_type>::GetType() const {return m_iType;}


template <class entity_type>
void Goal<entity_type>::ReactivateIfFailed()
{
	// if m_iStatus is failed this method sets it to inactive so that the goal
	// will be reactivated (replanned) on the next update-step.
	if (hasFailed())
	{
		m_iStatus = inactive;
	}
}


template <class entity_type>
void Goal<entity_type>::ActivateIfInactive()
{
	if (isInactive())
	{
		Activate();
	}
}


#endif // GOAL_H_INCLUDED
