#pragma once
#include "KhuGleSprite.h"
#include <stdlib.h>

class Brick : public CKhuGleSprite
{
private :
	int hp;
	bool isBroken;

public :
	Brick(int stype, int ctype, CKgLine line, 
		KgColor24 fgColor, bool bFill, int nSliceOrWidth = 100);
	~Brick();


	void setIsBroken(bool isBroken);
	bool getIsBroken();

	void setBrickHP(int hp);
	int getBrickHP();

	void Render();
};

