#include <cstdio>
#include <GameState.h>

GameState::GameState() {
  grid.resize(9, std::vector(16, std::vector(9, false)));
}

/**
 * Check if a cube level is completed (1 cube per round)
 */
void GameState::checkCompleteLevels() {
  {
    for (int y = 0; y < 16; y++) {
      bool levelComplete = true;
      for (int x = 0; x < 9 && levelComplete; x++) {
        for (int z = 0; z < 9 && levelComplete; z++) {
          if (!grid[x][y][z]) levelComplete = false;
        }
      }
      if (levelComplete) {
        collapseLevel(y);
        y--;
      }
    }
  }
}

/**
 * Destroy the level and add point
 * @param level
 */
void GameState::collapseLevel(int level) {
  gameState.startShake();
  {
    for (int y = level; y < 15; y++) {
      for (int x = 0; x < 9; x++) {
        for (int z = 0; z < 9; z++) {
          if (y + 3 < 16) {
            grid[x][y][z] = grid[x][y + 3][z];
          } else {
            ThrownCube thrownCube{};
            thrownCube.position = glm::vec3(x - 4, -4, z - 4);
            thrownCube.velocity = glm::vec3(
                (rand() % 200 - 100) / 100.0f,
                (rand() % 200 - 100) / 100.0f,
                (rand() % 200 - 100) / 100.0f
            );
            thrownCubes.push_back(thrownCube);
            grid[x][y][z] = false;
          }
        }
      }
    }
    points += COLLAPSE_POINT;
  }
}

/**
 * Update the falling speed
 */
void GameState::updateFallingSpeed() {
  switch (fallingSpeedLevel) {
    case 1: fallingSpeed = LEVEL_1_FALLING_SPEED; break;
    case 2: fallingSpeed = LEVEL_2_FALLING_SPEED; break;
    case 3: fallingSpeed = LEVEL_3_FALLING_SPEED; break;
    case 4: fallingSpeed = LEVEL_4_FALLING_SPEED; break;
    case 5: fallingSpeed = LEVEL_5_FALLING_SPEED; break;
    case 6: fallingSpeed = LEVEL_6_FALLING_SPEED; break;
    default: fallingSpeed = INITIAL_FALLING_SPEED; break;
  }
}

/**
 * Check if block can be placed
 * @param x
 * @param y
 * @param z
 * @return
 */
bool GameState::canPlaceBlock(float x, float y, float z) {
  // Check each cube in the 3x3x3 block
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < 3; k++) {
        if (checkCollision(x + i - 1, y + j, z + k - 1)) {
          return false;
        }
      }
    }
  }
  return true;
}

/**
 * Check collision by using the initial block of the cube
 *
 * @param x
 * @param y
 * @param z
 * @return
 */
bool GameState::checkCollision(float x, float y, float z) {
  int gridX = static_cast<int>(std::round(x + 4));
  int gridY = static_cast<int>(std::round(y + 4));
  int gridZ = static_cast<int>(std::round(z + 4));

  // Invalid position, collision impossible
  if (gridX < 0 || gridX >= 9 || gridZ < 0 || gridZ >= 9) {
    return true;
  }
  // Grid pozition
  if (y <= -4.0f) {
    return true;
  }
  // Other cubes
  if (gridY >= 0 && gridY < 16) {
    return grid[gridX][gridY][gridZ];
  }

  return false;
}

/**
 * Check for game over
 * @return
 */
bool GameState::canSpawnNewBlock() {
  // Check if spawn area (top area) is clear
  for (int x = 0; x < 3; x++) {
    for (int z = 0; z < 3; z++) {
      int gridX = x + 3;
      int gridY = TETRIS_INITIAL_BLOCK_HEIGHT + 4;
      int gridZ = z + 3;

      if (grid[gridX][gridY][gridZ]) {
        return false;
      }
    }
  }
  return true;
}

void GameState::landBlock() {
  if (gameOver) {
    return;
  }

  float currentX = round(blockPosition[0]);
  float currentZ = round(blockPosition[1]);
  float currentY = round(blockHeight);

  // Convert into grid positions
  int gridX = static_cast<int>(currentX + 4);
  int gridY = static_cast<int>(currentY + 4);
  int gridZ = static_cast<int>(currentZ + 4);
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      for (int z = 0; z < 3; z++) {
        int posX = gridX + (x - 1);
        int posY = gridY + y;
        int posZ = gridZ + (z - 1);

        if (posX >= 0 && posX < 9 && posY >= 0 && posY < 16 && posZ >= 0 && posZ < 9) {
            grid[posX][posY][posZ] = true;
          }
      }
    }
  }

  checkCompleteLevels();
  // Spawn new block at initial position
  blockPosition[0] = TETRIS_INITIAL_BLOCK_POSITION;
  blockPosition[1] = TETRIS_INITIAL_BLOCK_POSITION;
  blockHeight = TETRIS_INITIAL_BLOCK_HEIGHT;
  // Check for game over by checking spawn area
  if (!canSpawnNewBlock()) {
    gameOver = true;
  }
}

/**
 * Start Shake Effect
 */
void GameState::startShake() {
  shakeActive = true;
  shakeTimer = 0.0f;
}

