#include "PongGame.h"

PongGame::PongGame() : CKhuGleWin(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	InitScene();
	InitResource();
}

PongGame::~PongGame() {

}

void PongGame::OnConnectServer() {
	while (true) {
		char sendBuffer[BUFFER_SIZE] = { '0','0','0','0','0','0',';' }; // index 4 : matching, index 5 : opponent
		if (readySignal) {
			sendBuffer[4] = '1';
		}
		if (isGameStart) {
			if (!opponentPlayer) {
				sendBuffer[2] = (rightPlayerMove[0]) ? '1' : '0';
				sendBuffer[3] = (rightPlayerMove[1]) ? '1' : '0';
			}
			else {
				sendBuffer[0] = (leftPlayerMove[0]) ? '1' : '0';
				sendBuffer[1] = (leftPlayerMove[1]) ? '1' : '0';
			}
		}
		send(sock, sendBuffer, BUFFER_SIZE, 0);

		if (recv(sock, recvBuffer, BUFFER_SIZE, 0) == SOCKET_ERROR) {
			std::cout << "Disconnected Server" << std::endl;
			break;
		}
		else {
			if (!isMatched) {
				if (recvBuffer[4] == '1') {
					isMatched = true;
					opponentPlayer = (recvBuffer[5] == '0') ? false : true;
					startSignal += 1;
				}
			}
			if (isGameStart) {
				if (!opponentPlayer) {
					leftPlayer->m_fgColor = RED;
					rightPlayer->m_fgColor = BLUE;
					opponentInput[0] = (recvBuffer[0] == '1') ? true : false;
					opponentInput[1] = (recvBuffer[1] == '1') ? true : false;
				}
				else {
					leftPlayer->m_fgColor = BLUE;
					rightPlayer->m_fgColor = RED;
					opponentInput[0] = (recvBuffer[2] == '1') ? true : false;
					opponentInput[1] = (recvBuffer[3] == '1') ? true : false;
				}
			}
		}
	}
}

void PongGame::OffConnectServer() {
	clientThread->join();
	closesocket(sock);
	WSACleanup();
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
		sock = socket(PF_INET, SOCK_STREAM, 0);
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

	opponentPlayer = false;
	opponentInput[0] = false;
	opponentInput[1] = false;
	readySignal = false;
	isMatched = false;

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
	leftPlayer = new Player(GP_STYPE_RECT, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(LAYER_MARGIN, 250), CKgPoint(LAYER_MARGIN + PLAYER_WIDTH, 250 + PLAYER_HEIGHT)), WHITE, true, PLAYER_WIDTH);
	leftPlayer->SetPhysics(CKgVector2D(600, 600), 200);

	rightPlayer = new Player(GP_STYPE_RECT, GP_CTYPE_KINEMATIC, CKgLine(CKgPoint(SCREEN_WIDTH - LAYER_MARGIN * 2, 250), CKgPoint(SCREEN_WIDTH - LAYER_MARGIN * 2 + PLAYER_WIDTH, 250 + PLAYER_HEIGHT)), KG_COLOR_24_RGB(255, 255, 255), true, PLAYER_WIDTH);
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
	delete singleGameButton;
	delete multiGameButton;
	m_pGameLayer->AddChild(leftPlayer);
	m_pGameLayer->AddChild(rightPlayer);
	m_pGameLayer->AddChild(ball);
}

void PongGame::ResetGame() {
	ball->MoveTo(600, 20);
	ball->m_Velocity = CKgVector2D(300 * (((leftScore + rightScore + 1) % 2 == 0) ? -1 : 1), 300);
}

void PongGame::MoveObject() {
	ball->MoveBy(ball->m_Velocity.x * m_ElapsedTime, ball->m_Velocity.y * m_ElapsedTime);
	leftPlayer->Move(leftPlayerMove[0], leftPlayerMove[1], m_ElapsedTime);
	rightPlayer->Move(rightPlayerMove[0], rightPlayerMove[1], m_ElapsedTime);
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
	double v11 = max(0.0, min(line.Dot(line), line.Dot(lineTball))) / line.Dot(line);
	CKgVector2D projection = v11 * line;
	CKgVector2D projectionPoint = lineTball - projection;
	return projectionPoint;
}

bool PongGame::IsOverlapped(CKhuGleSprite* s1) { //공과 다른 선 객체와의 충돌 확인
	CKgVector2D project = GetProjectionResult(s1);
	double distance = (ball->m_Radius + s1->m_nWidth / 2.0f);
	double overlapped = CKgVector2D::abs(project) - distance;
	return (overlapped <= 0);
}

void PongGame::MakePlayerAndBallCollision(CKhuGleSprite* s1) { //가상의 원을 만들어 공과 충돌처리
	CKgVector2D project = GetProjectionResult(s1);
	CKgVector2D normal = (1 / CKgVector2D::abs(project)) * project;
	CKgVector2D line = CKgVector2D(s1->m_lnLine.End.X - s1->m_lnLine.Start.X, s1->m_lnLine.End.Y - s1->m_lnLine.Start.Y);
	double direction = CKgVector2D::abs(line);
	double speed = CKgVector2D::abs(ball->m_Velocity);

	double theta = acos(ball->m_Velocity.Dot(line) / (direction * speed));
	double square = sin(theta);
	CKgVector2D reaction = (speed * 2 * square) * normal;
	ball->m_Velocity += reaction;
	ball->m_Velocity = (s1->m_Mass / ball->m_Mass) * speed * ((1 / CKgVector2D::abs(ball->m_Velocity)) * ball->m_Velocity);

	double distance = (ball->m_Radius + s1->m_nWidth / 2.0f);
	double offset = distance - CKgVector2D::abs(project);
	CKgVector2D offsetVector = (offset / 2) * project;
	ball->MoveBy(offsetVector.x, offsetVector.y);
}

void PongGame::StrictBallPosition() {
	if (ball->m_rtBoundBox.Top < 0) {
		ball->m_rtBoundBox.Top = 0;
		ball->m_rtBoundBox.Bottom = ball->m_Radius * 2;
	}
	if (ball->m_rtBoundBox.Bottom > SCREEN_HEIGHT) {
		ball->m_rtBoundBox.Top = SCREEN_HEIGHT - ball->m_Radius * 2;
		ball->m_rtBoundBox.Bottom = SCREEN_HEIGHT;
	}
	if (ball->m_rtBoundBox.Left < 0) {
		ball->m_rtBoundBox.Left = 0;
		ball->m_rtBoundBox.Right = ball->m_Radius * 2;
	}
	if (ball->m_rtBoundBox.Right > SCREEN_WIDTH) {
		ball->m_rtBoundBox.Right = SCREEN_WIDTH - ball->m_Radius * 2;
		ball->m_rtBoundBox.Right = SCREEN_WIDTH;
	}
	ball->m_Center.x = (ball->m_rtBoundBox.Left + ball->m_rtBoundBox.Right) / 2;
	ball->m_Center.y = (ball->m_rtBoundBox.Bottom + ball->m_rtBoundBox.Top) / 2;

	ball->m_Acceleration.x = ball->m_Velocity.x * m_AirResistance.x;
	ball->m_Acceleration.y = ball->m_Velocity.y * m_AirResistance.y;

	ball->m_Velocity.x += ball->m_Acceleration.x * m_ElapsedTime;
	ball->m_Velocity.y += ball->m_Acceleration.y * m_ElapsedTime;

	if (ball->m_Velocity.x > 1000) {
		ball->m_Velocity.x = 1000;
	}
	if (ball->m_Velocity.y > 1000) {
		ball->m_Velocity.y = 1000;
	}
	if (ball->m_Velocity.x < -1000) {
		ball->m_Velocity.x = -1000;
	}
	if (ball->m_Velocity.y < -1000) {
		ball->m_Velocity.y = -1000;
	}
}

void PongGame::CheckBallCollision() {
	if (IsOverlapped(rightPlayer)) {
		MakePlayerAndBallCollision(rightPlayer);
	}
	else if (IsOverlapped(leftPlayer)) {
		MakePlayerAndBallCollision(leftPlayer);
	}
	for (int i = 0; i < 4; i++) { //벽과의 충돌
		bool overlapped = IsOverlapped(maps[i]);
		if (overlapped) {
			switch (i)
			{
			case 0:
				rightScore++;
				ResetGame();
				break;
			case 1:
				leftScore++;
				ResetGame();
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
				DrawSceneTextPos(message.c_str(), CKgPoint(300, 100), WHITE, 100);
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
		if (!opponentPlayer) {
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
				MoveObject();
				StrictBallPosition();
				CheckInput();
				CheckBallCollision();
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
			if (gameType == GameType::MULTI) {
				if (m_bKeyPressed[VK_SPACE] && !readySignal) {
					readySignal = true;
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
				if (ConnectServer()) {
					char buffer[BUFFER_SIZE] = { '0','0','0','0','0','0',';'};
					send(sock, buffer, BUFFER_SIZE, 0);
					clientThread = new std::thread(&PongGame::OnConnectServer,this);
				}
				else {
					isGameStart = true;
					isGameEnd = true;
				}
			}
			m_bMousePressed[0] = false;
		}
	}
}