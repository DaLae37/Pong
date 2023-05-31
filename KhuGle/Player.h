#pragma once
#include "stdafx.h"
#include "KhuGleSprite.h"

class Player : public CKhuGleSprite
{
public :
	Player(int stype, int ctype, CKgLine line, KgColor24 fgColor, bool bFill, int nSliceOrWidth);
	~Player();

	void SetPhysics(CKgVector2D velocity, double mass);
	void Move(bool up, bool down, float deltaTime);
};