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
#include "commands/discoveryPacket.h"
#include "mockSimplePacketComs.h"
#include <Arduino.h>
#include <unity.h>

void test_discover_with_packetid_equal_to_discovery_packetid() {
  MockSimplePacketComs coms;
  DiscoveryPacket p(&coms);
  std::array<std::uint8_t, 60> buffer{
    1, DISCOVERY_PACKET_ID, RESOURCE_TYPE_ANALOG_IN, ATTACHMENT_POINT_TYPE_PIN, 1};
  p.event((float *)buffer.data());
  TEST_ASSERT_EQUAL(STATUS_REJECTED_GENERIC, buffer[0]);
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_discover_with_packetid_equal_to_discovery_packetid);
  UNITY_END();
}

void loop() {
}
