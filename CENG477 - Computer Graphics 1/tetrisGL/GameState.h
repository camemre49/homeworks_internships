#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <string>
#include <vector>

#include "GameConstants.h"
struct ThrownCube;

class  GameState {
public:
  // general state
  int points = 0;
  bool gameOver = false;
  std::vector<std::vector<std::vector<bool>>> grid;

  // falling
  int fallingSpeedLevel = TETRIS_INITIAL_FALLING_SPEED_LEVEL; // Start at level 2
  float fallingSpeed = INITIAL_FALLING_SPEED; // 1 block fall in 2 seconds
  float fallTimer = 0.0f;

  // camera angle
  float cameraAngle = INITIAL_CAMERA_ANGLE;
  float targetCameraAngle = 0.0f;
  float rotationSpeed = ROTATION_SPEED;

  // key presses
  std::string lastKeyPressed;
  std::string currentView = "Front";
  bool keyIsPressed = false;

  // current block positions
  float blockPosition[2] = {TETRIS_INITIAL_BLOCK_POSITION, TETRIS_INITIAL_BLOCK_POSITION};
  float blockHeight = TETRIS_INITIAL_BLOCK_HEIGHT;

  // Shake Effect State
  bool shakeActive = false;
  float shakeTimer = 0.0f;
  // Store thrown cubes
  std::vector<ThrownCube> thrownCubes;


  GameState();

  void checkCompleteLevels();
  void collapseLevel(int level);
  void updateFallingSpeed();
  bool checkCollision(float x, float y, float z);
  bool canPlaceBlock(float x, float y, float z);
  bool canSpawnNewBlock();
  void landBlock();
  void startShake();
};

inline GameState gameState;

// Thrown cube structure
struct ThrownCube {
  glm::vec3 position;
  glm::vec3 velocity;
};

#endif //GAMESTATE_H
