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
#pragma once

#if defined(Arduino_h)
#include <Arduino.h>
#endif
#include "../config.h"
#include "commands/DiscoveryPacket.h"
#include <ESP32Encoder.h>
#include <ESP32Servo.h>
#include <Esp32SimplePacketComs.h>
#include <Esp32WifiManager.h>
#include <Preferences.h>
#include <SimplePacketComs.h>

enum state_t {
  Startup,
  WaitForConnect,
  run
  // Add more states here and be sure to add them to the cycle
};

/**
 * RobotControlCenter is the main class for the 2000 level student code
 *
 * This class is the starting point for all the code. Student code is
 * housed in StudentsRobot class
 * @see StudentsRobot
 *
 * This class managed the overall state machine for dealing with connecting and
 * maintaining connection to the WiFi, constructing the StudentsRobot class, and managing
 * the StudentsRobot calls to updates its state machine as well as its PID loops.
 *
 */
class RobotControlCenter {
  public:
  /**
   * RobotControlCenter constructor.
   *
   * The name is used by the SimplePacketComs stack to locate your specific
   * robot on the network.
   */
  RobotControlCenter();

  virtual ~RobotControlCenter() {
  }

  /**
   * Pulse the loop function from the main thread.
   *
   * This function is called over and over by the INO loop().
   */
  void loop();

  protected:
  /**
   * Internal setup function to set up all objects.
   *
   * This function is called as part of the state machine by the object.
   */
  void setup();

  /**
   * 	The fast loop actions
   *
   * 	This should be run every loop and is internally gated for fast opperation.
   */
  void fastLoop();

  private:
  int64_t lastPrint = 0;

#if defined(USE_WIFI)
  // SimplePacketComs implementation using WiFi
  UDPSimplePacket coms;

  // Wifi stack managment state machine
  WifiManager manager;
#endif

  // State machine state
  state_t state = Startup;
};
