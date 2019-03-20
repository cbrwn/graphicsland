#include "game.h"

int main() {
	Game g;
	int result = g.init("john cena", 1280, 720);
	if (result != 0)
		return result;
	g.loop();
	return 0;
}