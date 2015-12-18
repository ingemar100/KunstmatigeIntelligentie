#include "SteeringBehaviors.h"
#include "Vehicle.h"
#include "GameWorld.h"
#include "BaseGameEntity.h"
#include <cassert>


using std::string;
using std::vector;


//------------------------- ctor -----------------------------------------
//
//------------------------------------------------------------------------
SteeringBehavior::SteeringBehavior(Vehicle* agent) :


	m_pVehicle(agent),
	m_iFlags(0),
	m_dWeightWander(200),
	m_dViewDistance(50),
	m_Deceleration(normal),
	m_dWanderDistance(WanderDist),
	m_dWanderJitter(WanderJitterPerSec),
	m_dWanderRadius(WanderRad),
	m_dWeightSeek(200),
	m_dWeightFlee(200),
	m_dWeightArrive(200),
	m_dWeightPursuit(200)


{
	//stuff for the wander behavior
	double theta = RandFloat() * TwoPi;

	//create a vector to a target position on the wander circle
	m_vWanderTarget = Vector2D(m_dWanderRadius * cos(theta),
		m_dWanderRadius * sin(theta));

	//create a Path
	m_pPath = new Path();
	m_pPath->LoopOn();

}

//---------------------------------dtor ----------------------------------
SteeringBehavior::~SteeringBehavior() { delete m_pPath; }


/////////////////////////////////////////////////////////////////////////////// CALCULATE METHODS 


//----------------------- Calculate --------------------------------------
//
//  calculates the accumulated steering force according to the method set
//  in m_SummingMethod
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Calculate()
{
	//reset the steering force
	m_vSteeringForce.Zero();

	//use space partitioning to calculate the neighbours of this vehicle
	//if switched on. If not, use the standard tagging system
	//calculate neighbours in cell-space if any of the following 3 group
	//behaviors are switched on

	m_vSteeringForce = CalculatePrioritized(); 


	return m_vSteeringForce;
}



//--------------------- AccumulateForce ----------------------------------
//
//  This function calculates how much of its max steering force the 
//  vehicle has left to apply and then applies that amount of the
//  force to add.
//------------------------------------------------------------------------
bool SteeringBehavior::AccumulateForce(Vector2D &RunningTot,
	Vector2D ForceToAdd)
{

	//calculate how much steering force the vehicle has used so far
	double MagnitudeSoFar = RunningTot.Length();

	//calculate how much steering force remains to be used by this vehicle
	double MagnitudeRemaining = m_pVehicle->MaxForce() - MagnitudeSoFar;

	//return false if there is no more force left to use
	if (MagnitudeRemaining <= 0.0) return false;

	//calculate the magnitude of the force we want to add
	double MagnitudeToAdd = ForceToAdd.Length();

	//if the magnitude of the sum of ForceToAdd and the running total
	//does not exceed the maximum force available to this vehicle, just
	//add together. Otherwise add as much of the ForceToAdd vector is
	//possible without going over the max.
	if (MagnitudeToAdd < MagnitudeRemaining)
	{
		RunningTot += ForceToAdd;
	}

	else
	{
		//add it to the steering force
		RunningTot += (Vec2DNormalize(ForceToAdd) * MagnitudeRemaining);
	}

	return true;
}



//---------------------- CalculatePrioritized ----------------------------
//
//  this method calls each active steering behavior in order of priority
//  and acumulates their forces until the max steering force magnitude
//  is reached, at which time the function returns the steering force 
//  accumulated to that  point
//------------------------------------------------------------------------
Vector2D SteeringBehavior::CalculatePrioritized()
{
	Vector2D force;


	if (On(evade))
	{
		assert(m_pTargetAgent1 && "Evade target not assigned");

		force = Evade(m_pTargetAgent1) * m_dWeightEvade;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}


	if (On(flee))
	{
		//force = Flee(m_pVehicle->World()->Crosshair()) * m_dWeightFlee;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(seek))
	{
		//force = Seek(m_pVehicle->World()->Crosshair()) * m_dWeightSeek;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}


	if (On(arrive))
	{
		//force = Arrive(m_pVehicle->World()->Crosshair(), m_Deceleration) * m_dWeightArrive;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(wander))
	{
		force = Wander() * m_dWeightWander;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(pursuit))
	{
		assert(m_pTargetAgent1 && "pursuit target not assigned");

		force = Pursuit(m_pTargetAgent1) * m_dWeightPursuit;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}



	return m_vSteeringForce;
}

/////////////////////////////////////////////////////////////////////////////// START OF BEHAVIORS

//------------------------------- Seek -----------------------------------
//
//  Given a target, this behavior returns a steering force which will
//  direct the agent towards the target
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Seek(Vector2D TargetPos)
{
	Vector2D DesiredVelocity = Vec2DNormalize(TargetPos - m_pVehicle->Pos())
		* m_pVehicle->MaxSpeed();

	return (DesiredVelocity - m_pVehicle->Velocity());
}

//----------------------------- Flee -------------------------------------
//
//  Does the opposite of Seek
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Flee(Vector2D TargetPos)
{
	//only flee if the target is within 'panic distance'. Work in distance
	//squared space.
	/* const double PanicDistanceSq = 100.0f * 100.0;
	if (Vec2DDistanceSq(m_pVehicle->Pos(), target) > PanicDistanceSq)
	{
	return Vector2D(0,0);
	}
	*/

	Vector2D DesiredVelocity = Vec2DNormalize(m_pVehicle->Pos() - TargetPos)
		* m_pVehicle->MaxSpeed();

	return (DesiredVelocity - m_pVehicle->Velocity());
}

//--------------------------- Arrive -------------------------------------
//
//  This behavior is similar to seek but it attempts to arrive at the
//  target with a zero velocity
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Arrive(Vector2D     TargetPos,
	Deceleration deceleration)
{
	Vector2D ToTarget = TargetPos - m_pVehicle->Pos();

	//calculate the distance to the target
	double dist = ToTarget.Length();

	if (dist > 0)
	{
		//because Deceleration is enumerated as an int, this value is required
		//to provide fine tweaking of the deceleration..
		const double DecelerationTweaker = 0.3;

		//calculate the speed required to reach the target given the desired
		//deceleration
		double speed = dist / ((double)deceleration * DecelerationTweaker);

		//make sure the velocity does not exceed the max
		speed = min(speed, m_pVehicle->MaxSpeed());

		//from here proceed just like Seek except we don't need to normalize 
		//the ToTarget vector because we have already gone to the trouble
		//of calculating its length: dist. 
		Vector2D DesiredVelocity = ToTarget * speed / dist;

		return (DesiredVelocity - m_pVehicle->Velocity());
	}

	return Vector2D(0, 0);
}

//------------------------------ Pursuit ---------------------------------
//
//  this behavior creates a force that steers the agent towards the 
//  evader
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Pursuit(const Vehicle* evader)
{
	//if the evader is ahead and facing the agent then we can just seek
	//for the evader's current position.
	Vector2D ToEvader = evader->Pos() - m_pVehicle->Pos();

	double RelativeHeading = m_pVehicle->Heading().Dot(evader->Heading());

	if ((ToEvader.Dot(m_pVehicle->Heading()) > 0) &&
		(RelativeHeading < -0.95))  //acos(0.95)=18 degs
	{
		return Seek(evader->Pos());
	}

	//Not considered ahead so we predict where the evader will be.

	//the lookahead time is propotional to the distance between the evader
	//and the pursuer; and is inversely proportional to the sum of the
	//agent's velocities
	double LookAheadTime = ToEvader.Length() /
		(m_pVehicle->MaxSpeed() + evader->Speed());

	//now seek to the predicted future position of the evader
	return Seek(evader->Pos() + evader->Velocity() * LookAheadTime);
}


//----------------------------- Evade ------------------------------------
//
//  similar to pursuit except the agent Flees from the estimated future
//  position of the pursuer
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Evade(const Vehicle* pursuer)
{
	/* Not necessary to include the check for facing direction this time */

	Vector2D ToPursuer = pursuer->Pos() - m_pVehicle->Pos();

	//uncomment the following two lines to have Evade only consider pursuers 
	//within a 'threat range'
	const double ThreatRange = 100.0;
	if (ToPursuer.LengthSq() > ThreatRange * ThreatRange) return Vector2D();

	//the lookahead time is propotional to the distance between the pursuer
	//and the pursuer; and is inversely proportional to the sum of the
	//agents' velocities
	double LookAheadTime = ToPursuer.Length() /
		(m_pVehicle->MaxSpeed() + pursuer->Speed());

	//now flee away from predicted future position of the pursuer
	return Flee(pursuer->Pos() + pursuer->Velocity() * LookAheadTime);
}


//--------------------------- Wander -------------------------------------
//
//  This behavior makes the agent wander about randomly
//------------------------------------------------------------------------
Vector2D SteeringBehavior::Wander()
{
	//this behavior is dependent on the update rate, so this line must
	//be included when using time independent framerate.
	double JitterThisTimeSlice = m_dWanderJitter * m_pVehicle->TimeElapsed();

	//first, add a small random vector to the target's position
	m_vWanderTarget += Vector2D(RandomClamped() * JitterThisTimeSlice,
		RandomClamped() * JitterThisTimeSlice);

	//reproject this new vector back on to a unit circle
	m_vWanderTarget.Normalize();

	//increase the length of the vector to the same as the radius
	//of the wander circle
	m_vWanderTarget *= m_dWanderRadius;

	//move the target into a position WanderDist in front of the agent
	Vector2D target = m_vWanderTarget + Vector2D(m_dWanderDistance, 0);

	//project the target into world space
	Vector2D Target = PointToWorldSpace(target,
		m_pVehicle->Heading(),
		m_pVehicle->Side(),
		m_pVehicle->Pos());

	//and steer towards it
	return Target - m_pVehicle->Pos();
}

