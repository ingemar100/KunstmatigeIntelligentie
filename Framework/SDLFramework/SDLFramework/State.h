#pragma once

template <class entity_type>
class State
{
public:
	virtual ~State() {}

	// Executed logic for when state is entered
	virtual void Enter(entity_type*) = 0;

	// Logic statements for normal update function
	virtual void Execute(entity_type*) = 0;

	// Executed logic for when state is exited
	virtual void Exit(entity_type*) = 0;
};