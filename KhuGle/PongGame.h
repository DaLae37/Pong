#pragma once
#include "stdafx.h"
#include "Player.h"

class PongGame : public CKhuGleWin
{
private:
	//Game Manager
	GameType gameType;
	bool isSetGameType;
	bool isGameStart;
	bool isGameEnd;
	bool isConnected;
	bool leftPlayerMove[2]; //UP, DOWN
	bool rightPlayerMove[2]; //UP, DOWN
	int leftScore;
	int rightScore;
	int startSignal;

	//UI
	CKhuGleSprite* singleGameButton;
	CKhuGleSprite* multiGameButton;

	//Object
	CKhuGleSprite* ball;
	CKhuGleSprite* maps[4]; //TOP, RIGHT, BOTTOM, LEFT º®

	//Player
	Player* leftPlayer;
	Player* rightPlayer;
	bool opponentPlayer; //LEFT = false, RIGHT = true
	bool opponentInput[2]; //UP, DOWN

	CKhuGleLayer* m_pGameLayer;
	CKgPoint m_LButtonStart, m_LButtonEnd;
	int m_nLButtonStatus;

public:
	PongGame();
	~PongGame();

	void Update();

	//Game Manager
	void InitScene();
	void InitResource();

	void CreateGame();

	//Server
	bool ConnectServer();

	//UI
	void RenderUI();

	//Player
	void CheckInput();

	//Collision
	bool CheckMouseInRect(CKhuGleSprite* rect);
	CKgVector2D GetProjectionResult(CKhuGleSprite* s1);
	double GetLineCollisionResult(CKhuGleSprite* s1);
	bool GetAABBCollisionResult(CKhuGleSprite* s1);
	void MakePlayerAndBallCollision(CKhuGleSprite* s1);


	void CollisionCheck();

};