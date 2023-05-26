#pragma once
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <ws2tcpip.h>
#define BUFFERSIZE 1024

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  _DEBUG

#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>

#include "KhuGleWin.h"