#include "game.h"

#ifdef _WIN32
#include <crtdbg.h>
#endif

int main()
{
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Game* g = new Game();
	int init = g->init("videogame", 854, 480, 8, false);
	if (init != 0)
		return init;
	g->loop();
	delete g;
	return 0;
}