#include "Brick.h"

Brick::Brick(int stype, int ctype, CKgLine line, KgColor24 fgColor, bool bFill, int nSliceOrWidth) : 
	CKhuGleSprite(stype, ctype, line, fgColor, bFill, nSliceOrWidth)
{
	isBroken = false;
	int r = rand() % 100 + 1;
	if (r < 7) {
		hp = 3;
	}
	else if(r < 15){
		hp = 2;
	}
	else {
		hp = 1;
	}
	setBrickHP(hp);
}

Brick::~Brick() {

}

void Brick::Render() {
	if (!isBroken) {
		CKhuGleSprite::Render();
	}
}

void Brick::setIsBroken(bool isBroken) {
	this->isBroken = isBroken;
}

bool Brick::getIsBroken() {
	return isBroken;
}

void Brick::setBrickHP(int hp) {
	this->hp = hp;
	switch (hp)
	{
	case 1 :
		this->m_fgColor = KG_COLOR_24_RGB(0, 255, 255);
		break;
	case 2 :
		this->m_fgColor = KG_COLOR_24_RGB(0, 128, 128);
		break;
	case 3 :
		this->m_fgColor = KG_COLOR_24_RGB(0, 64, 64);
		break;
	default:
		this->m_fgColor = KG_COLOR_24_RGB(255, 255, 255);
		break;
	}

	if (hp <= 0) {
		isBroken = true;
	}
}

int Brick::getBrickHP() {
	return hp;
}