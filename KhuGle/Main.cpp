#include "stdafx.h"
#include "MainScene.h"

int main()
{
	srand(time(NULL));
	MainScene* main = new MainScene(SCREEN_WIDTH, SCREEN_HEIGHT);
	KhuGleWinInit(main);
	return 0;
}