#include "Vehicle.h"
#include "C2DMatrix.h"
#include "Geometry.h"
#include "SteeringBehaviors.h"
#include "Transformations.h"
#include "GameWorld.h"
#include "VehicleOwnedStates.h"

using std::vector;
using std::list;


//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
Vehicle::Vehicle(GameWorld* world,
	Vector2D position,
	double    rotation,
	Vector2D velocity,
	double    mass,
	double    max_force,
	double    max_speed,
	double    max_turn_rate,
	double    scale,
	std::string texture) : MovingEntity(position,
		scale,
		velocity,
		max_speed,
		Vector2D(sin(rotation), -cos(rotation)),
		mass,
		Vector2D(scale, scale),
		max_turn_rate,
		max_force),

	m_pWorld(world),
	m_dTimeElapsed(0.0)
{

	//set up the steering behavior class
	m_pSteering = new SteeringBehavior(this);

	_texture = texture;

	stateMachine = new StateMachine<Vehicle>(this);
}


//---------------------------- dtor -------------------------------------
//-----------------------------------------------------------------------
Vehicle::~Vehicle()
{
	delete m_pSteering;
}

//------------------------------ Update ----------------------------------
//
//  Updates the vehicle's position from a series of steering behaviors
//------------------------------------------------------------------------
void Vehicle::Update(double time_elapsed)
{
	stateMachine->Update();

	//update the time elapsed
	m_dTimeElapsed = time_elapsed;

	//keep a record of its old position so we can update its cell later
	//in this method
	Vector2D OldPos = Pos();


	Vector2D SteeringForce;

	//calculate the combined force from each steering behavior in the 
	//vehicle's list
	SteeringForce = m_pSteering->Calculate();

	//Acceleration = Force/Mass
	Vector2D acceleration = SteeringForce / m_dMass;

	//update velocity
	m_vVelocity += acceleration * time_elapsed;

	//make sure vehicle does not exceed maximum velocity
	m_vVelocity.Truncate(m_dMaxSpeed);

	Vector2D testVelocity = Vector2D(3, 3);
	//update the position
	m_vPos += m_vVelocity * time_elapsed;

	//update the heading if the vehicle has a non zero velocity
	if (m_vVelocity.LengthSq() > 0.00000001)
	{
		m_vHeading = Vec2DNormalize(m_vVelocity);

		m_vSide = m_vHeading.Perp();
	}

	//EnforceNonPenetrationConstraint(this, World()->Agents());

	//treat the screen as a toroid
	WrapAround(m_vPos, m_pWorld->cxClient(), m_pWorld->cyClient());

}


//-------------------------------- Render -------------------------------------
//-----------------------------------------------------------------------------
void Vehicle::Render()
{
	SDL_Texture* texture = FWApplication::GetInstance()->LoadTexture(_texture);
	FWApplication::GetInstance()->DrawTexture(texture, m_vPos.x, m_vPos.y, 85, 93);

	FWApplication::GetInstance()->SetColor(Color(0, 0, 0, 255));
	if (stateMachine->isInState(*VehicleEvadeState::Instance())) {
		FWApplication::GetInstance()->JustDrawText("Evade", 50, 50);
	}
	if (stateMachine->isInState(*VehicleWanderState::Instance())) {
		FWApplication::GetInstance()->JustDrawText("Wander", 50, 50);
	}

	FWApplication::GetInstance()->SetColor(Color(255,255,255, 0));

	FWApplication::GetInstance()->RemoveTexture(texture);

}
