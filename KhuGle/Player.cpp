#include "Player.h"

Player::Player(int stype, int ctype, CKgLine line, KgColor24 fgColor, bool bFill, int nSliceOrWidth) :
	CKhuGleSprite(stype, ctype, line, fgColor, bFill, nSliceOrWidth) {

}

Player::~Player() {

}

void Player::SetPhysics(CKgVector2D velocity, double mass) {
	m_Velocity = velocity;
	m_Mass = mass;
}

void Player::Move(bool up, bool down, float deltaTime) {
	if (up) {
		MoveBy(0, -m_Velocity.y * deltaTime);
	}
	if (down) {
		MoveBy(0, m_Velocity.y * deltaTime);
	}

	if (m_rtBoundBox.Top < SCREEN_MARGIN) { //왼쪽 화면을 넘어가지 못하게 값 조정
		m_rtBoundBox.Top = SCREEN_MARGIN;
	}
	if (m_rtBoundBox.Bottom > SCREEN_HEIGHT - SCREEN_MARGIN) {//오른쪽 화면을 넘어가지 못하게 값 조정
		m_rtBoundBox.Bottom = SCREEN_HEIGHT - SCREEN_MARGIN;
	}
}