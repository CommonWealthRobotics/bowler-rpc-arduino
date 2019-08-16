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
#include "RobotControlCenter.h"
#include <Arduino.h>

#if defined(PLATFORM_ESP32)
static int64_t getTime() {
  return esp_timer_get_time();
}
#elif defined(PLATFORM_TEENSY)
static uint32_t getTime() {
  return millis();
}
#endif

void RobotControlCenter::loop() {
  if (getTime() - lastLoopTime > 500 || getTime() < lastLoopTime // check for the wrap over case
  ) {
    switch (state) {
    case Startup:
      setup();
      state = WaitForConnect;
      break;

    case WaitForConnect:
#if defined(USE_WIFI)
      if (manager.getState() == Connected)
#endif
        state = run; // begin the main operation loop
      break;

    default:
      break;
    }

    // ensure 0.5 ms spacing *between* reads for Wifi to transact
    lastLoopTime = getTime();
  }

  if (state != Startup) {
    // If this is run before the sensor reads, the I2C will fail because the time it takes to send
    // the UDP causes a timeout
    fastLoop(); // Run wifi after State machine on all states
  }
}

RobotControlCenter::RobotControlCenter() {
  state = Startup;
}

void RobotControlCenter::setup() {
  if (state != Startup)
    return;
  state = WaitForConnect;
#if defined(USE_WIFI)
  manager.setupAP();
#elif defined(USE_HID)
#else
  Serial.begin(115200);
#endif

#if defined(USE_WIFI)
  // Attach coms
  coms.attach(new DiscoveryPacket(&coms));
#endif
}

void RobotControlCenter::fastLoop() {
  // Do not run before startp
  if (state == Startup) {
    return;
  }

#if defined(USE_WIFI)
  manager.loop();
  if (manager.getState() == Connected) {
    coms.server();
  } else {
    return;
  }
#elif defined(USE_HID)
  coms.server();
#endif
}
