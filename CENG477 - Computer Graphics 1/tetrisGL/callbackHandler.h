
#ifndef CALLBACKHANDLER_H
#define CALLBACKHANDLER_H
#include <string>

void handleRotationInput(const std::string& direction, float angleDelta);
void handleFallingSpeedAdjustment(const std::string& direction);
void handleMovementInput(const std::string& direction, float deltaX, float deltaZ);

#endif //CALLBACKHANDLER_H
