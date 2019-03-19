#include "game.h"

int main() {
	Game g;
	int result = g.init("john cena", 1920, 1080);
	if (result != 0)
		return result;
	g.loop();
	return 0;
}