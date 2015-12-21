#pragma once
#pragma once

#include <cassert>

#include "State.h"

template <class entity_type>
class StateMachine
{
public:
	StateMachine(entity_type* owner) :
		owner_pointer_(owner),
		current_state_pointer_(NULL),
		previous_state_(NULL),
		global_state_pointer_(NULL)
	{}

	virtual ~StateMachine() {};

	// Initialization methods
	void SetCurrentState(State<entity_type>* s) { 
		current_state_pointer_ = s; 
		s->Enter(owner_pointer_);
	}
	void SetGlobalState(State<entity_type>* s) { global_state_pointer_ = s; }
	void SetPreviousState(State<entity_type>* s) { previous_state_ = s; }

	void Update()const {
		// If a global state exists, call its execute method; same for current state
		if (global_state_pointer_) global_state_pointer_->Execute(owner_pointer_);
		if (current_state_pointer_) current_state_pointer_->Execute(owner_pointer_);
	}

	void Render()const {
		// If a global state exists, call its render method; same for current state
		if (global_state_pointer_) global_state_pointer_->Render(owner_pointer_);
		if (current_state_pointer_) current_state_pointer_->Render(owner_pointer_);
	}

	// Change to a new state
	void ChangeState(State<entity_type>* newStatePointer)
	{
		assert(newStatePointer);

		// Keep a record of previous state
		previous_state_ = current_state_pointer_;

		// Call the exit method of existing state
		current_state_pointer_->Exit(owner_pointer_);

		// Change state to new state
		current_state_pointer_ = newStatePointer;

		// Call entry method of new state
		current_state_pointer_->Enter(owner_pointer_);
	}

	// Change state back to previous state
	void RevertToPreviousState() { ChangeState(previous_state_); }

	// Returns true if current type of current state is equal to
	// type of class passed as a parameter. 
	bool isInState(const State<entity_type>& st)const {
		return typeid(*current_state_pointer_) == typeid(st);
	}

	State<entity_type>*  CurrentState()  const { return current_state_pointer_; }
	State<entity_type>*  GlobalState()   const { return global_state_pointer_; }
	State<entity_type>*  PreviousState() const { return previous_state_; }

private:
	// A pointer to the agent that owns this instance
	entity_type* owner_pointer_;

	// Container for current state
	State<entity_type>*	current_state_pointer_;

	// A record of the last state the agent was in
	State<entity_type>*   previous_state_;

	// Called every time the FSM is updated
	State<entity_type>*   global_state_pointer_;
};

