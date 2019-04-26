#include "game.h"

#ifdef _WIN32
#include <crtdbg.h>
#endif

int main()
{
#ifdef _WIN32
	// memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Game* g = new Game();
	// 854x480 window, 8 MSAA samples & postprocessing enabled
	int init = g->init("videogame", 854, 480, 8, true);
	if (init != 0)
		return init;
	// start game loop
	g->loop();
	delete g;
	return 0;
}