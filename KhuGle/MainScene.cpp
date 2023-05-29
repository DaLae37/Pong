#include "MainScene.h"

MainScene::MainScene() : CKhuGleWin(SCREEN_WIDTH, SCREEN_HEIGHT) {
	m_pScene = new CKhuGleScene(SCREEN_WIDTH, SCREEN_HEIGHT, KG_COLOR_24_RGB(95, 95, 95));
	layer = new CKhuGleLayer(SCREEN_WIDTH - SCREEN_MARGIN, SCREEN_HEIGHT - SCREEN_MARGIN, KG_COLOR_24_RGB(0, 0, 0), CKgPoint(30, 30));

	m_pScene->AddChild(layer);
}

MainScene::~MainScene() {
	delete layer;
}

void MainScene::Update() {
	m_pScene->m_Children.clear();
}