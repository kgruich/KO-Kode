#include "Engine.h"

#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
	}

	Engine engine;
	Engine::gameLoop();
	SDL_Quit();
	return 0;
}
