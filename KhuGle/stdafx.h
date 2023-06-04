#pragma once
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <ws2tcpip.h>
#define BUFFER_SIZE 8

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  _DEBUG

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include "KhuGleWin.h"
#include "resource.h"

#define PROGRAM_NAME "Pong"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_MARGIN 60
#define LAYER_MARGIN (SCREEN_MARGIN + 30)
#define PLAYER_WIDTH 15
#define PLAYER_HEIGHT 100
#define BALL_RADIUS 15

#define BLACK KG_COLOR_24_RGB(0, 0, 0)
#define WHITE KG_COLOR_24_RGB(255, 255, 255)
#define RED KG_COLOR_24_RGB(255, 0, 0)
#define BLUE KG_COLOR_24_RGB(0, 0, 255)
#define YELLOW KG_COLOR_24_RGB(255, 255, 0)
#define GRAY KG_COLOR_24_RGB(95, 95, 95)

enum class GameType {
	NONE,
	SINGLE,
	MULTI
};