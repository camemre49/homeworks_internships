#include <string>
#include "callbackHandler.h"
#include "GameState.h"

void handleRotationInput(const std::string& direction, float angleDelta) {
  gameState.targetCameraAngle += angleDelta;
  gameState.lastKeyPressed = direction;

  if (direction == "H") {
    if (gameState.currentView == "Front") gameState.currentView = "Left";
    else if (gameState.currentView == "Left") gameState.currentView = "Back";
    else if (gameState.currentView == "Back") gameState.currentView = "Right";
    else if (gameState.currentView == "Right") gameState.currentView = "Front";
  } else if (direction == "K") {
    if (gameState.currentView == "Front") gameState.currentView = "Right";
    else if (gameState.currentView == "Right") gameState.currentView = "Back";
    else if (gameState.currentView == "Back") gameState.currentView = "Left";
    else if (gameState.currentView == "Left") gameState.currentView = "Front";
  }
}

void handleFallingSpeedAdjustment(const std::string& direction) {
  if (direction == "S") {
    gameState.fallingSpeedLevel = std::min(6, gameState.fallingSpeedLevel + 1);
  } else if (direction == "W") {
    gameState.fallingSpeedLevel = std::max(1, gameState.fallingSpeedLevel - 1);
  }
  gameState.updateFallingSpeed();
  gameState.lastKeyPressed = direction;
}

void handleMovementInput(const std::string& direction, float deltaX, float deltaZ) {
  float newX = gameState.blockPosition[0] + deltaX;
  float newZ = gameState.blockPosition[1] + deltaZ;

  if (gameState.canPlaceBlock(newX, gameState.blockHeight, newZ)) {
    gameState.blockPosition[0] = newX;
    gameState.blockPosition[1] = newZ;
  }
  gameState.lastKeyPressed = direction;
}