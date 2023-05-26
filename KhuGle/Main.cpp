#include "stdafx.h"
#include "PongGame.h"

int main()
{
	srand(time(NULL));
	PongGame* game = new PongGame(1280, 720);
	KhuGleWinInit(game);
	return 0;
}