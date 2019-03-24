#include "game.h"

#ifdef _WIN32
#include <crtdbg.h>
#endif

int main() {
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Game g;
	int result = g.init("john cena", 854, 480);
	if (result != 0)
		return result;
	g.loop();
	return 0;
}