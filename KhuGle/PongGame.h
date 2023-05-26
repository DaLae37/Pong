#pragma once
#include "stdafx.h"
#include "Brick.h"

class PongGame : public CKhuGleWin
{
private:
	bool isStart;
	short HP;
	int remainBricks;
	bool isCollision;

public:
	CKhuGleSprite* player;
	CKhuGleSprite* ball;
	CKhuGleSprite* maps[4]; //TOP, RIGHT, BOTTOM, LEFT º®
	std::vector<Brick*>bricks; // 5 x 10

	void CreateLevel();
	void CollisionCheck();
	void InGameInputCheck();
	void UIRender();

	bool ConnectServer();

	PongGame(int nW, int nH);
	void Update();
	CKhuGleLayer* m_pGameLayer;
	CKgPoint m_LButtonStart, m_LButtonEnd;
	int m_nLButtonStatus;

	CKgVector2D GetProjectionResult(CKhuGleSprite* s1);
	double GetLineCollisionResult(CKhuGleSprite* s1);
	bool GetAABBCollisionResult(CKhuGleSprite* s1);
	void MakeVritualCricleToCollision(CKhuGleSprite* s1);
};