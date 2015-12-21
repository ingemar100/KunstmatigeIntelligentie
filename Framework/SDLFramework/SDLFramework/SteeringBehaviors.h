#ifndef STEERINGBEHAVIORS_H
#define STEERINGBEHAVIORS_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   SteeringBehaviors.h
//
//  Desc:   class to encapsulate steering behaviors for a Vehicle
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <windows.h>
#include <string>
#include <list>

#include "Vector2D.h"
#include "Path.h"


class Vehicle;
class CController;
class Wall2D;
class BaseGameEntity;
class BaseGameEntity;




//--------------------------- Constants ----------------------------------

//the radius of the constraining circle for the wander behavior
const double WanderRad = 1.2;
//distance the wander circle is projected in front of the agent
const double WanderDist = 2.0;
//the maximum amount of displacement along the circle each frame
const double WanderJitterPerSec = 80.0;

//used in path following
const double WaypointSeekDist = 20;



//------------------------------------------------------------------------

class SteeringBehavior
{

private:

	enum behavior_type
	{
		none = 0x00000,
		seek = 0x00002,
		flee = 0x00004,
		arrive = 0x00008,
		wander = 0x00010,
		pursuit = 0x00800,
		evade = 0x01000,
		flocking = 0x08000
	};

private:


	//a pointer to the owner of this instance
	Vehicle*     m_pVehicle;

	//the steering force created by the combined effect of all
	//the selected behaviors
	Vector2D    m_vSteeringForce;

	//these can be used to keep track of friends, pursuers, or prey
	Vehicle*     m_pTargetAgent;

	//the current target
	Vector2D    m_vTarget;



	//the current position on the wander circle the agent is
	//attempting to steer towards
	Vector2D     m_vWanderTarget;

	//explained above
	double        m_dWanderJitter;
	double        m_dWanderRadius;
	double        m_dWanderDistance;


	//multipliers. These can be adjusted to effect strength of the  
	//appropriate behavior. Useful to get flocking the way you require
	//for example.
	double        m_dWeightWander;
	double        m_dWeightSeek;
	double        m_dWeightFlee;
	double        m_dWeightArrive;
	double        m_dWeightPursuit;
	double        m_dWeightEvade;
	double        m_dWeightSeparation;
	double        m_dWeightAlignment;
	double        m_dWeightCohesion;

	//how far the agent can 'see'
	double        m_dViewDistance;

	//pointer to any current path
	Path*          m_pPath;

	//the distance (squared) a vehicle has to be from a path waypoint before
	//it starts seeking to the next waypoint
	double        m_dWaypointSeekDistSq;

	//any offset used for formations or offset pursuit
	Vector2D     m_vOffset;

	//binary flags to indicate whether or not a behavior should be active
	int           m_iFlags;


	//Arrive makes use of these to determine how quickly a vehicle
	//should decelerate to its target
	enum Deceleration { slow = 3, normal = 2, fast = 1 };

	//default
	Deceleration m_Deceleration;

	//this function tests if a specific bit of m_iFlags is set
	bool      On(behavior_type bt) { return (m_iFlags & bt) == bt; }

	bool      AccumulateForce(Vector2D &sf, Vector2D ForceToAdd);




	/* .......................................................

	BEGIN BEHAVIOR DECLARATIONS

	.......................................................*/


	//this behavior moves the agent towards a target position
	Vector2D Seek(Vector2D TargetPos);

	//this behavior returns a vector that moves the agent away
	//from a target position
	Vector2D Flee(Vector2D TargetPos);

	//this behavior is similar to seek but it attempts to arrive 
	//at the target position with a zero velocity
	Vector2D Arrive(Vector2D     TargetPos,
		Deceleration deceleration);

	//this behavior predicts where an agent will be in time T and seeks
	//towards that point to intercept it.
	Vector2D Pursuit(const Vehicle* agent);


	//this behavior attempts to evade a pursuer
	Vector2D Evade(const Vehicle* agent);

	//this behavior makes the agent wander about randomly
	Vector2D Wander();


	/* .......................................................

	END BEHAVIOR DECLARATIONS

	.......................................................*/

	//calculates and sums the steering forces from any active behaviors
	Vector2D CalculatePrioritized();






public:

	SteeringBehavior(Vehicle* agent);

	virtual ~SteeringBehavior();

	//calculates and sums the steering forces from any active behaviors
	Vector2D Calculate();

	void TagNeighbors();

	Vector2D Separation(const std::vector<Vehicle*>& neighbors);

	Vector2D Alignment(const std::vector<Vehicle*>& neighbors);

	Vector2D Cohesion(const std::vector<Vehicle*>& neighbors);


	void SetTarget(const Vector2D t) { m_vTarget = t; }

	void SetTargetAgent(Vehicle* Agent) { m_pTargetAgent = Agent; }
	Vehicle* getTargetAgent() { return m_pTargetAgent; }


	Vector2D Force()const { return m_vSteeringForce; }


	void FleeOn() { m_iFlags |= flee; }
	void SeekOn() { m_iFlags |= seek; }
	void ArriveOn() { m_iFlags |= arrive; }
	void WanderOn() { m_iFlags |= wander; }
	void PursuitOn() { m_iFlags |= pursuit; }
	void EvadeOn() { m_iFlags |= evade; }
	void FlockingOn() { m_iFlags |= flocking; }

	void FleeOff() { if (On(flee))   m_iFlags ^= flee; }
	void SeekOff() { if (On(seek))   m_iFlags ^= seek; }
	void ArriveOff() { if (On(arrive)) m_iFlags ^= arrive; }
	void WanderOff() { if (On(wander)) m_iFlags ^= wander; }
	void PursuitOff() { if (On(pursuit)) m_iFlags ^= pursuit; }
	void EvadeOff() { if (On(evade)) m_iFlags ^= evade; }
	void FlockingOff() { if (On(flocking)) m_iFlags ^= flocking; }

	bool isFleeOn() { return On(flee); }
	bool isSeekOn() { return On(seek); }
	bool isArriveOn() { return On(arrive); }
	bool isWanderOn() { return On(wander); }
	bool isPursuitOn() { return On(pursuit); }
	bool isEvadeOn() { return On(evade); }
	bool isFlockingOn() { return On(flocking); }

	double WanderJitter()const { return m_dWanderJitter; }
	double WanderDistance()const { return m_dWanderDistance; }
	double WanderRadius()const { return m_dWanderRadius; }

};




#endif