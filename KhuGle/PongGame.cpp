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

	m_Gravity = CKgVector2D(0., 98.);
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
	leftPlayer = new Player(GP_STYPE_RECT, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(LAYER_MARGIN, 250), CKgPoint(LAYER_MARGIN + PLAYER_WIDTH, 250 + PLAYER_HEIGHT)), WHITE, true, 0);
	leftPlayer->SetPhysics(CKgVector2D(200, 200), 200);

	rightPlayer = new Player(GP_STYPE_RECT, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(SCREEN_WIDTH - LAYER_MARGIN * 2, 250), CKgPoint(SCREEN_WIDTH - LAYER_MARGIN * 2 + PLAYER_WIDTH, 250 + PLAYER_HEIGHT)), KG_COLOR_24_RGB(255, 255, 255), true, 0);
	rightPlayer->SetPhysics(CKgVector2D(200, 200), 200);

	//Object
	ball = new CKhuGleSprite(GP_STYPE_ELLIPSE, GP_CTYPE_DYNAMIC, CKgLine(CKgPoint(280, 280), CKgPoint(300, 300)), YELLOW, true, BALL_RADIUS);
	ball->m_Velocity = CKgVector2D(300, 300);

	maps[0] = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(0, 0), CKgPoint(0, 659)), KG_COLOR_24_RGB(0, 0, 0), false, 0);
	maps[1] = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(1219, 0), CKgPoint(1219, 659)), KG_COLOR_24_RGB(0, 0, 0), false, 0);
	maps[2] = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(0, 0), CKgPoint(1220, 0)), KG_COLOR_24_RGB(0, 0, 0), false, 0);
	maps[3] = new CKhuGleSprite(GP_STYPE_LINE, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(0, 659), CKgPoint(1220, 659)), KG_COLOR_24_RGB(0, 0, 0), false, 0);
}

void PongGame::CreateGame() {
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

bool PongGame::CheckMouseInRect(CKhuGleSprite* rect) {
	if (rect == NULL) {
		return false;
	}
	else {
		return (rect->m_rtBoundBox.Left <= m_MousePosX) &&
			(rect->m_rtBoundBox.Right >= m_MousePosX) &&
			(rect->m_rtBoundBox.Top <= m_MousePosY) &&
			(rect->m_rtBoundBox.Bottom >= m_MousePosY);
	}
}

CKgVector2D PongGame::GetProjectionResult(CKhuGleSprite* s1) { //Projection Vector
	CKgVector2D line = CKgVector2D(s1->m_lnLine.End.X - s1->m_lnLine.Start.X,
		s1->m_lnLine.End.Y - s1->m_lnLine.Start.Y);
	CKgVector2D lineTball = CKgVector2D(ball->m_Center.x - s1->m_lnLine.Start.X,
		ball->m_Center.y - s1->m_lnLine.Start.Y);

	double v11 = line.Dot(line);
	double projectionRate = max(0, min(v11, line.Dot(lineTball))) / v11;

	CKgVector2D projection = CKgVector2D(s1->m_lnLine.Start) + projectionRate * line;
	return projection;
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
	CKgVector2D PosVec = project - ball->m_Center;
	double posVecY = PosVec.y;
	double posVecX = PosVec.x;
	double Distance = CKgVector2D::abs(PosVec);
	if (Distance == 0) Distance = 1E-6;
	CKgVector2D Normal = (1. / Distance) * PosVec;

	double kx = (ball->m_Velocity.x - s1->m_Velocity.x);
	double ky = (ball->m_Velocity.y - s1->m_Velocity.y);

}

void PongGame::CollisionCheck() {
	for (int i = 0; i < 4; i++) { //벽과의 충돌

		double overlapped = GetLineCollisionResult(maps[i]);

		if (overlapped <= 0) {
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
				return;
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
			CheckInput();
			leftPlayer->Move(leftPlayerMove[0], leftPlayerMove[1], m_ElapsedTime);
			rightPlayer->Move(rightPlayerMove[0], rightPlayerMove[1], m_ElapsedTime);
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
			if (CheckMouseInRect(singleGameButton)) {
				gameType = GameType::SINGLE;
				isSetGameType = true;
				CreateGame();
			}
			else if (CheckMouseInRect(multiGameButton)) {
				gameType = GameType::MULTI;
				isSetGameType = true;
				CreateGame();
			}
			m_bMousePressed[0] = false;
		}
	}
}