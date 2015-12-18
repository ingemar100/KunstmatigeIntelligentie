#ifndef GameWorld_H
#define GameWorld_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   GameWorld.h
//
//  Desc:   All the environment data and methods for the Steering
//          Behavior projects. This class is the root of the project's
//          update and render calls (excluding main of course)
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <windows.h>
#include <vector>

#include "Vector2D.h"
#include "BaseGameEntity.h"
#include "vehicle.h"


class Obstacle;
class Wall2D;
class Path;


typedef std::vector<BaseGameEntity*>::iterator  ObIt;


class GameWorld
{
private:

	//a container of all the moving entities
	std::vector<Vehicle*>         m_Vehicles;

	//local copy of client window dimensions
	int                           m_cxClient,
		m_cyClient;

	//keeps track of the average FPS
	double                         m_dAvFrameTime;



public:

	GameWorld(int cx, int cy);

	~GameWorld();

	void  Update(double time_elapsed);

	void  Render();

	const std::vector<Vehicle*>&        Agents() { return m_Vehicles; }


	int   cxClient()const { return m_cxClient; }
	int   cyClient()const { return m_cyClient; }

};



#endif