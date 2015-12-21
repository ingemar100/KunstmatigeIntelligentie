#ifndef VEHICLE_H
#define VEHICLE_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   Vehicle.h
//
//  Desc:   Definition of a simple vehicle that uses steering behaviors
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include "MovingEntity.h"
#include "Vector2D.h"
#include "FWApplication.h"
#include "StateMachine.h"

#include <vector>
#include <list>
#include <string>

class GameWorld;
class SteeringBehavior;



class Vehicle : public MovingEntity
{

private:

	//a pointer to the world data. So a vehicle can access any obstacle,
	//path, wall or agent data
	GameWorld*            m_pWorld;

	//the steering behavior class
	SteeringBehavior*     m_pSteering;

	//keeps a track of the most recent update time. (some of the
	//steering behaviors make use of this - see Wander)
	double                m_dTimeElapsed;


	//disallow the copying of Vehicle types
	Vehicle(const Vehicle&);
	Vehicle& operator=(const Vehicle&);

	std::string _texture;
	StateMachine<Vehicle>* stateMachine;

	double viewDistance = 150;

	friend class VehicleWanderState;
	friend class VehicleEvadeState;

public:

	Vehicle(GameWorld* world,
		Vector2D position,
		double    rotation,
		Vector2D velocity,
		double    mass,
		double    max_force,
		double    max_speed,
		double    max_turn_rate,
		double    scale,
		std::string texture);

	~Vehicle();

	//updates the vehicle's position and orientation
	void        Update(double time_elapsed);

	void        Render();


	//-------------------------------------------accessor methods
	SteeringBehavior*const  Steering()const { return m_pSteering; }
	GameWorld*const         World()const { return m_pWorld; }


	double       TimeElapsed()const { return m_dTimeElapsed; }

	StateMachine<Vehicle>*	GetFSM()const { return stateMachine; }
};



#endif