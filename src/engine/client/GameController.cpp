#include "GameController.h"

GameController::GameController(InputManager& rInputMgr) : m_rInputMgr(rInputMgr) {}
GameController::~GameController() {}

void GameController::collectInput() { m_rInputMgr.CollectInputs(); }
