#pragma once
#include "stdafx.h"

class MainScene : public CKhuGleWin
{
private :
	CKhuGleLayer* layer;
public :
	MainScene();
	~MainScene();

	void Update();
};