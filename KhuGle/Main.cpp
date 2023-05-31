#include "stdafx.h"
#include "PongGame.h"

int main()
{
	srand(time(NULL));
	PongGame* game = new PongGame();
	KhuGleWinInit(game);
	return 0;
}