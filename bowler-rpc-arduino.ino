/*
 * This file is part of bowler-rpc-arduino.
 *
 * bowler-rpc-arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bowler-rpc-arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with bowler-rpc-arduino.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <ESP32Encoder.h>
#include <ESP32Servo.h>
#include <Esp32SimplePacketComs.h>
#include <Preferences.h>
#include <SimplePacketComs.h>
#include <server/NameCheckerServer.h>

#include "config.h"
#include "src/RobotControlCenter.h"

RobotControlCenter *controlCenter;
void setup() {
  controlCenter = new RobotControlCenter();
}

void loop() {
  controlCenter->loop(); // run the state machine pulse
}
