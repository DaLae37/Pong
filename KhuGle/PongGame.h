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

	//Server
	bool opponentPlayer; //LEFT = false, RIGHT = true
	bool opponentInput[2]; //UP, DOWN
	bool readySignal; //Ingame Ready
	bool isMatched;
	char recvBuffer[BUFFER_SIZE];
	SOCKET sock;
	SOCKADDR_IN addr;
	std::thread *clientThread;

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

	void StartGame();
	void ResetGame();

	void MoveObject();

	//Server
	bool ConnectServer();
	void OnConnectServer();
	void OffConnectServer();

	//UI
	void RenderUI();

	//Player
	void CheckInput();

	//Collision
	CKgVector2D GetProjectionResult(CKhuGleSprite* s1);
	bool IsPointInRect(CKgPoint point, CKhuGleSprite* rect);
	bool IsOverlapped(CKhuGleSprite* s1);
	void MakePlayerAndBallCollision(CKhuGleSprite* s1);
	void CheckBallCollision();
	void StrictBallPosition();
};