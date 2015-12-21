#pragma once
#include "State.h"

class Vehicle;

class VehicleWanderState : public State<Vehicle>
{
private:
	VehicleWanderState() {}

	// Copy constructor and assignment should be private
	VehicleWanderState(const VehicleWanderState&);
	VehicleWanderState& operator=(const VehicleWanderState&);
public:
	static VehicleWanderState* Instance();

	virtual void Enter(Vehicle* vehicle);
	virtual void Execute(Vehicle* vehicle);
	virtual void Exit(Vehicle* vehicle);
};

class VehicleEvadeState : public State<Vehicle>
{
private:
	VehicleEvadeState() {}

	// Copy constructor and assignment should be private
	VehicleEvadeState(const VehicleEvadeState&);
	VehicleEvadeState& operator=(const VehicleEvadeState&);
public:
	static VehicleEvadeState* Instance();

	virtual void Enter(Vehicle* vehicle);
	virtual void Execute(Vehicle* vehicle);
	virtual void Exit(Vehicle* vehicle);
};


class VehiclePursuitState : public State<Vehicle>
{
private:
	VehiclePursuitState() {}

	// Copy constructor and assignment should be private
	VehiclePursuitState(const VehiclePursuitState&);
	VehiclePursuitState& operator=(const VehiclePursuitState&);
public:
	static VehiclePursuitState* Instance();

	virtual void Enter(Vehicle* vehicle);
	virtual void Execute(Vehicle* vehicle);
	virtual void Exit(Vehicle* vehicle);
};