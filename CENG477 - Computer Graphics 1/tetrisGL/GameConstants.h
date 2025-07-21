#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>

constexpr int TETRIS_INITIAL_BLOCK_POSITION = 0;
constexpr float TETRIS_INITIAL_BLOCK_HEIGHT = 9.0f;
constexpr int TETRIS_INITIAL_FALLING_SPEED_LEVEL = 2;
constexpr float LEVEL_1_FALLING_SPEED = 0.0f;
constexpr float LEVEL_2_FALLING_SPEED = 0.25f;
constexpr float LEVEL_3_FALLING_SPEED = 0.5f;
constexpr float LEVEL_4_FALLING_SPEED = 1.0f;
constexpr float LEVEL_5_FALLING_SPEED = 2.0f;
constexpr float LEVEL_6_FALLING_SPEED = 4.0f;
constexpr float INITIAL_FALLING_SPEED = 0.5f;
constexpr int COLLAPSE_POINT = 243;
constexpr float INITIAL_CAMERA_ANGLE = 0.0f;
constexpr float ROTATION_SPEED = 2.0f;
constexpr glm::vec3 TRANSLATION_TO_MIDDLE_VECTOR = glm::vec3(-0.5, -3, -0.5);
constexpr float INITIAL_EYE_Y_POSITION = 10.0f;
constexpr float INITIAL_EYE_GRID_DISTANCE = 24.0f;
constexpr float INITIAL_LIGHT_Y_POSITION = 10.0f;
constexpr float INITIAL_LIGHT_GRID_DISTANCE = 7.0f;
constexpr int INITIAL_WINDOW_WIDTH = 600;
constexpr int INITIAL_WINDOW_HEIGHT = 1000;
constexpr float SHAKE_DURATION = 1.0f;
constexpr float SHAKE_FREQUENCY = 20.0f;
constexpr float SHAKE_MAX_INTENSITY = 0.05;
#endif //CONSTANTS_H
