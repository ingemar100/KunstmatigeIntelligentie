#include <iostream>
#include "Config.h"
#include "FWApplication.h"
#include <SDL_events.h>
#include "SDL_timer.h"
#include <time.h>
#include "Vehicle.h"
#include "GameWorld.h"

int main(int args[])
{
	//auto window = Window::CreateSDLWindow();
	FWApplication* application = FWApplication::GetInstance();
	if (!application->GetWindow())
	{
		LOG("Couldn't create window...");
		return EXIT_FAILURE;
	}
	
	application->SetTargetFPS(60);
	application->SetColor(Color(255, 10, 40, 255));

	GameWorld* world = new GameWorld(1000,800);
	
	double dt = 0;
	Uint32 lastTime = SDL_GetTicks();

	srand(time(NULL));
	//while (true){}
	while (application->IsRunning())
	{
		application->StartTick();


		Uint32 ticks = SDL_GetTicks();
		dt = ticks - lastTime;
		lastTime = ticks;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				application->Quit();
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){

				default:
					break;
				}
			}
		}
		
		application->SetColor(Color(0, 0, 0, 255));
		application->JustDrawText("Welcome to KMint", 800 / 2, 600 / 2);
		
		// For the background
		application->SetColor(Color(255, 255, 255, 255));

		application->UpdateGameObjects();
		//world->Update(1);
		world->Update(dt / 1000);
		application->RenderGameObjects();
		world->Render();
		application->EndTick();
	}
		
	return EXIT_SUCCESS;
}