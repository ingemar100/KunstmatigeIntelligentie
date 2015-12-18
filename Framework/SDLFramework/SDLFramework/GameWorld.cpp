#include "GameWorld.h"
#include "Vehicle.h"
#include "SteeringBehaviors.h"

#include <list>
using std::list;


//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------
GameWorld::GameWorld(int cx, int cy) :

	m_cxClient(cx),
	m_cyClient(cy),
	m_dAvFrameTime(0)
{


	Vector2D SpawnPos = Vector2D(cx / 2.0 + RandomClamped()*cx / 2.0,
		cy / 2.0 + RandomClamped()*cy / 2.0);

	//setup the agents
	Vehicle* hare = new Vehicle(this,
		SpawnPos,                 //initial position
		RandFloat()*TwoPi,        //start rotation
		Vector2D(0, 0),            //velocity
		1,          //mass
		400,     //max force
		50,             //max velocity
		Pi, //max turn rate
		3,
		"rabbit-3.png");        //scale)
	hare->Steering()->WanderOn();
	hare->SetMaxSpeed(150);
	m_Vehicles.push_back(hare);

	//determine a random starting position
	SpawnPos = Vector2D(cx / 2.0 + RandomClamped()*cx / 2.0,
		cy / 2.0 + RandomClamped()*cy / 2.0);

	Vehicle* cow = new Vehicle(this,
		SpawnPos,                 //initial position
		RandFloat()*TwoPi,        //start rotation
		Vector2D(0, 0),            //velocity
		1,          //mass
		400,     //max force
		50,             //max velocity
		Pi, //max turn rate
		3,
		"lemmling_Cartoon_cow.png");        //scale

	cow->Steering()->PursuitOn(hare);

	m_Vehicles.push_back(cow);
}


//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
GameWorld::~GameWorld()
{
	for (unsigned int a = 0; a<m_Vehicles.size(); ++a)
	{
		delete m_Vehicles[a];
	}

}


//----------------------------- Update -----------------------------------
//------------------------------------------------------------------------
void GameWorld::Update(double time_elapsed)
{

	//create a smoother to smooth the framerate
	const int SampleRate = 10;
	static Smoother<double> FrameRateSmoother(SampleRate, 0.0);

	m_dAvFrameTime = FrameRateSmoother.Update(time_elapsed);


	//update the vehicles
	for (unsigned int a = 0; a<m_Vehicles.size(); ++a)
	{
		m_Vehicles[a]->Update(time_elapsed);
	}
}


//------------------------------ Render ----------------------------------
//------------------------------------------------------------------------
void GameWorld::Render()
{
	//render the agents
	for (unsigned int a = 0; a<m_Vehicles.size(); ++a)
	{
		m_Vehicles[a]->Render();
	}
}
