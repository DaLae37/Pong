#include "PongGame.h"

PongGame::PongGame() : CKhuGleWin(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	InitScene();
	InitResource();
}

PongGame::~PongGame() {

}

void PongGame::InitScene() {
	gameType = GameType::NONE;

	isSetGameType = false;
	isGameStart = false;
	isGameEnd = false;
	isConnected = false;
	leftPlayerMove[0] = false;
	leftPlayerMove[1] = false;
	rightPlayerMove[0] = false;
	rightPlayerMove[1] = false;

	leftScore = 0;
	rightScore = 0;
	startSignal = 0;

	m_AirResistance = CKgVector2D(0.1, 0.1);

	m_pScene = new CKhuGleScene(SCREEN_WIDTH, SCREEN_HEIGHT, GRAY);

	m_pGameLayer = new CKhuGleLayer(SCREEN_WIDTH - SCREEN_MARGIN, SCREEN_HEIGHT - SCREEN_MARGIN, BLACK, CKgPoint(30, 30));
	m_pScene->AddChild(m_pGameLayer);
}

void PongGame::InitResource() {
	//UI
	singleGameButton = new CKhuGleSprite(GP_STYPE_RECT, GP_CTYPE_STATIC, CKgLine(CKgPoint(450, 350), CKgPoint(750, 450)), WHITE, true, 0);
	m_pGameLayer->AddChild(singleGameButton);

	multiGameButton = new CKhuGleSprite(GP_STYPE_RECT, GP_CTYPE_STATIC, CKgLine(CKgPoint(450, 500), CKgPoint(750, 600)), WHITE, true, 0);
	m_pGameLayer->AddChild(multiGameButton);

	//Player
	leftPlayer = new Player(GP_STYPE_RECT, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(LAYER_MARGIN, 250), CKgPoint(LAYER_MARGIN + PLAYER_WIDTH, 250 + PLAYER_HEIGHT)), WHITE, true, PLAYER_HEIGHT);
	leftPlayer->SetPhysics(CKgVector2D(600, 600), 200);

	rightPlayer = new Player(GP_STYPE_RECT, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(SCREEN_WIDTH - LAYER_MARGIN * 2, 250), CKgPoint(SCREEN_WIDTH - LAYER_MARGIN * 2 + PLAYER_WIDTH, 250 + PLAYER_HEIGHT)), KG_COLOR_24_RGB(255, 255, 255), true, PLAYER_HEIGHT);
	rightPlayer->SetPhysics(CKgVector2D(600, 600), 200);

	//Object
	ball = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_DYNAMIC, CKgLine(CKgPoint(600, 20), CKgPoint(620, 40)), YELLOW, true, BALL_RADIUS);
	ball->m_Velocity = CKgVector2D(300, 300);

	maps[0] = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(0, 0), CKgPoint(0, 659)), KG_COLOR_24_RGB(0, 0, 0), false, 0);
	maps[1] = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(1219, 0), CKgPoint(1219, 659)), KG_COLOR_24_RGB(0, 0, 0), false, 0);
	maps[2] = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(0, 0), CKgPoint(1220, 0)), KG_COLOR_24_RGB(0, 0, 0), false, 0);
	maps[3] = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(0, 659), CKgPoint(1220, 659)), KG_COLOR_24_RGB(0, 0, 0), false, 0);
}

void PongGame::StartGame() {
	for (int i = 0; i < m_pGameLayer->m_Children.size(); i++) {
		if (m_pGameLayer->m_Children[i] == singleGameButton) {
			m_pGameLayer->m_Children.erase(m_pGameLayer->m_Children.begin() + i);
		}
	}
	for (int i = 0; i < m_pGameLayer->m_Children.size(); i++) {
		if (m_pGameLayer->m_Children[i] == multiGameButton) {
			m_pGameLayer->m_Children.erase(m_pGameLayer->m_Children.begin() + i);
		}
	}
	m_pGameLayer->AddChild(leftPlayer);
	m_pGameLayer->AddChild(rightPlayer);
	m_pGameLayer->AddChild(ball);
}

void PongGame::ResetGame() {
	ball->MoveTo(600, 20);
	ball->m_Velocity = CKgVector2D(300 * (((leftScore + rightScore) % 2 == 0) ? -1 : 1), 300);
}

void PongGame::MoveObject() {
	if (ball->m_Velocity.x > 500) {
		ball->m_Velocity.x = 500;
	}
	if (ball->m_Velocity.y > 500) {
		ball->m_Velocity.y = 500;
	}
	ball->MoveBy(ball->m_Velocity.x * m_ElapsedTime, ball->m_Velocity.y * m_ElapsedTime);


	leftPlayer->Move(leftPlayerMove[0], leftPlayerMove[1], m_ElapsedTime);
	rightPlayer->Move(rightPlayerMove[0], rightPlayerMove[1], m_ElapsedTime);
}

bool PongGame::ConnectServer() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "Ver 2.2 Erorr" << std::endl;
		return 1;
	}

	PCWSTR serverName = L"server.dalae37.com";
	PADDRINFOW ipInfo;

	if (GetAddrInfoW(serverName, NULL, NULL, &ipInfo) == 0) {
		PIN_ADDR ip = &((PSOCKADDR_IN)ipInfo->ai_addr)->sin_addr;
		unsigned long ipName = ip->S_un.S_addr;
		SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
		SOCKADDR_IN addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = ipName;
		addr.sin_port = htons(3737);
		std::cout << "Server IP : " << inet_ntoa(addr.sin_addr) << std::endl;
		if (connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
			std::cout << "Server Connection Error" << std::endl;
			return false;
		}
	}
	else {
		std::cout << "Unknown Server Name : " << serverName << std::endl;
		return false;
	}
	return true;
}

bool PongGame::IsPointInRect(CKgPoint point, CKhuGleSprite* rect) {
	if (rect == NULL) {
		return false;
	}
	else {
		return (rect->m_rtBoundBox.Left <= point.X) &&
			(rect->m_rtBoundBox.Right >= point.X) &&
			(rect->m_rtBoundBox.Top <= point.Y) &&
			(rect->m_rtBoundBox.Bottom >= point.Y);
	}
}

CKgVector2D PongGame::GetProjectionResult(CKhuGleSprite* s1) { //Projection Vector
	CKgVector2D line = CKgVector2D(s1->m_lnLine.End.X - s1->m_lnLine.Start.X, s1->m_lnLine.End.Y - s1->m_lnLine.Start.Y);
	CKgVector2D lineTball = CKgVector2D(ball->m_Center.x - s1->m_lnLine.Start.X, ball->m_Center.y - s1->m_lnLine.Start.Y);
	double v11 = (line.x * lineTball.x + line.y * lineTball.y) / line.Dot(line);
	CKgVector2D projection = v11 * line;
	CKgVector2D projectionPoint = CKgVector2D(s1->m_lnLine.Start) + projection;
	return projectionPoint;
}

double PongGame::GetLineCollisionResult(CKhuGleSprite* s1) { //공과 다른 선 객체와의 충돌 확인
	CKgVector2D normal = ball->m_Center - GetProjectionResult(s1);
	double overlapped = CKgVector2D::abs(normal) - ball->m_Radius;
	return overlapped;
}

bool PongGame::GetAABBCollisionResult(CKhuGleSprite* s1) { //공과 다른 사각형 객체와의 충돌 확인
	return (s1->m_rtBoundBox.Left <= ball->m_Center.x + ball->m_Radius) &&
		(s1->m_rtBoundBox.Right >= ball->m_Center.x - ball->m_Radius) &&
		(s1->m_rtBoundBox.Top <= ball->m_Center.y + ball->m_Radius) &&
		(s1->m_rtBoundBox.Bottom >= ball->m_Center.y - ball->m_Radius);
}

void PongGame::MakePlayerAndBallCollision(CKhuGleSprite* s1) { //가상의 원을 만들어 공과 충돌처리
	CKgVector2D project = GetProjectionResult(s1);
	CKgVector2D PosVec = ball->m_Center - project;
	double distance = CKgVector2D::abs(PosVec);
	if (distance == 0) distance = 1E-6;
	CKgVector2D Normal = (1. / distance) * PosVec;

	double kx = abs(ball->m_Velocity.x - s1->m_Velocity.x);
	double ky = abs(ball->m_Velocity.y - s1->m_Velocity.y);
	double p = 2.0 * (Normal.x * kx + Normal.y * ky) / (ball->m_Mass + s1->m_Mass);

	ball->m_Velocity.x = ball->m_Velocity.x - p * s1->m_Mass * Normal.x;
	ball->m_Velocity.y = ball->m_Velocity.y - p * s1->m_Mass * Normal.y;
}

void PongGame::CheckBallCollision() {
	if (GetLineCollisionResult(rightPlayer) <= 0) {
		//ball->MoveBy(-PosVec.x * Overlapped / CKgVector2D::abs(PosVec), -PosVec.y * Overlapped / CKgVector2D::abs(PosVec));
		MakePlayerAndBallCollision(rightPlayer);
	}
	else if (GetLineCollisionResult(leftPlayer) <= 0) {
		MakePlayerAndBallCollision(leftPlayer);
	}
	for (int i = 0; i < 4; i++) { //벽과의 충돌
		double overlapped = GetLineCollisionResult(maps[i]);
		if (overlapped <= 0) {
			std::cout << "충돌" << std::endl;
			switch (i)
			{
			case 0:
				ball->m_Velocity.x = -ball->m_Velocity.x;
				break;
			case 1:
				ball->m_Velocity.x = -ball->m_Velocity.x;
				break;
			case 2:
				ball->m_Velocity.x = ball->m_Velocity.x;
				ball->m_Velocity.y = -ball->m_Velocity.y;
				break;
			case 3:
				ball->m_Velocity.x = ball->m_Velocity.x;
				ball->m_Velocity.y = -ball->m_Velocity.y;
				break;
			default:
				break;
			}
		}
	}
}

void PongGame::RenderUI() {
	if (isSetGameType) {
		if (isGameStart) {
			if (isGameEnd) {
				std::string message;
				if (leftScore >= 10) {
					message = "왼쪽 플레이어 승리!";
				}
				else {
					message = "오른쪽 플레이어 승리!";
				}
				DrawSceneTextPos(message.c_str(), CKgPoint(275, 100), WHITE, 100);
				DrawSceneTextPos("ESC를 누르면 종료됩니다", CKgPoint(425, 600), WHITE, 50);
			}
			std::string message = std::to_string(leftScore) + "  :  " + std::to_string(rightScore);
			DrawSceneTextPos("Pong (10점을 내는 사람이 승리합니다)", CKgPoint(0, 0), WHITE);
			DrawSceneTextPos(message.c_str(), CKgPoint(550, 50), WHITE, 75);
		}
		else {
			std::string currentReady = std::to_string(startSignal);
			std::string message = "준비가 되었으면 스페이스바를 눌러주세요 (" + currentReady + "/2)";
			DrawSceneTextPos(message.c_str(), CKgPoint(110, 75), WHITE, 70);
		}
		switch (gameType)
		{
		case GameType::SINGLE:
			break;
		case GameType::MULTI:
			break;
		default:
			break;
		}
	}
	else {
		DrawSceneTextPos("PONG", CKgPoint(450, 75), WHITE, 150);
		DrawSceneTextPos("싱글 게임", CKgPoint(500, 390), BLACK, 75);
		DrawSceneTextPos("멀티 게임", CKgPoint(500, 540), BLACK, 75);
	}
}

void PongGame::CheckInput() {
	rightPlayerMove[0] = m_bKeyPressed[VK_UP];
	rightPlayerMove[1] = m_bKeyPressed[VK_DOWN];
	leftPlayerMove[0] = m_bKeyPressed['W'];
	leftPlayerMove[1] = m_bKeyPressed['S'];

	if (gameType == GameType::SINGLE) {
		return;
	}
	else if (gameType == GameType::MULTI) {
		if (opponentPlayer) {
			leftPlayerMove[0] = opponentInput[0];
			leftPlayerMove[1] = opponentInput[1];
		}
		else {
			rightPlayerMove[0] = opponentInput[0];
			rightPlayerMove[1] = opponentInput[1];
		}
	}
}

void PongGame::Update() {
	CKhuGleWin::Update();
	m_pScene->Render();
	RenderUI();

	if (isSetGameType) {
		if (isGameStart) {
			if (isGameEnd) {
				if (m_bKeyPressed[VK_ESCAPE]) {
					PostQuitMessage(0);
					return;
				}
			}
			else {
				CheckInput();
				CheckBallCollision();
				MoveObject();
				if (leftScore >= 10 || rightScore >= 10) {
					isGameEnd = true;
				}
			}
		}
		else {
			if (gameType == GameType::SINGLE) {
				if (m_bKeyPressed[VK_SPACE]) {
					startSignal += 1;
					m_bKeyPressed[VK_SPACE] = false;
				}
			}

			if (startSignal >= 2) {
				isGameStart = true;
			}
		}
	}
	else {
		if (m_bMousePressed[0]) {
			if (IsPointInRect(CKgPoint(m_MousePosX, m_MousePosY), singleGameButton)) {
				gameType = GameType::SINGLE;
				isSetGameType = true;
				StartGame();
			}
			else if (IsPointInRect(CKgPoint(m_MousePosX, m_MousePosY), multiGameButton)) {
				gameType = GameType::MULTI;
				isSetGameType = true;
				StartGame();
			}
			m_bMousePressed[0] = false;
		}
	}
}