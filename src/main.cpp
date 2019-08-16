#include "RobotControlCenter.h"

RobotControlCenter *controlCenter;

void setup() {
  controlCenter = new RobotControlCenter();
}

void loop() {
  controlCenter->loop(); // run the state machine pulse
}
