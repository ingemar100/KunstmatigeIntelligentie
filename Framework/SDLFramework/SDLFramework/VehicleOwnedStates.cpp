#include "VehicleOwnedStates.h"
#include "Vehicle.h"
#include "SteeringBehaviors.h"
#include "GameWorld.h"


VehicleWanderState* VehicleWanderState::Instance()
{
	static VehicleWanderState instance;
	return &instance;
}

void VehicleWanderState::Enter(Vehicle* vehicle)
{
	vehicle->Steering()->WanderOn();
}

void VehicleWanderState::Execute(Vehicle* vehicle)
{
	//kijken of koe te dicht bij is
	//switch naar fleestate
	const double ThreatRange = vehicle->viewDistance;

	for (auto threat : vehicle->World()->Agents()) {
		if (threat != vehicle) {
			Vector2D pursuerDistance = threat->Pos() - vehicle->Pos();
			if (pursuerDistance.LengthSq() < ThreatRange * ThreatRange) {
				vehicle->Steering()->SetTargetAgent(threat);
				vehicle->GetFSM()->ChangeState(VehicleEvadeState::Instance());
				return;
			}
		}
	}
}

void VehicleWanderState::Exit(Vehicle* vehicle) {
	vehicle->Steering()->WanderOff();
}

/*** EVADE **/
VehicleEvadeState* VehicleEvadeState::Instance()
{
	static VehicleEvadeState instance;
	return &instance;
}

void VehicleEvadeState::Enter(Vehicle* vehicle)
{
	vehicle->Steering()->EvadeOn();
	vehicle->SetMaxSpeed(150);
}

void VehicleEvadeState::Execute(Vehicle* vehicle)
{
	//kijken of koe te ver weg is
	//switch naar wander
	const double ThreatRange = vehicle->viewDistance * 2;

	Vector2D pursuerDistance = vehicle->Steering()->getTargetAgent()->Pos() - vehicle->Pos();
	if (pursuerDistance.LengthSq() > ThreatRange * ThreatRange) {
		vehicle->GetFSM()->ChangeState(VehicleWanderState::Instance());
	}
}

void VehicleEvadeState::Exit(Vehicle* vehicle) {
	vehicle->Steering()->EvadeOff();
	vehicle->SetMaxSpeed(50);
}


/*** PURSUIT **/
VehiclePursuitState* VehiclePursuitState::Instance()
{
	static VehiclePursuitState instance;
	return &instance;
}

void VehiclePursuitState::Enter(Vehicle* vehicle)
{
	vehicle->Steering()->PursuitOn();
}

void VehiclePursuitState::Execute(Vehicle* vehicle)
{
	//arrive?
}

void VehiclePursuitState::Exit(Vehicle* vehicle) {
	vehicle->Steering()->PursuitOff();
}