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
#include "mockBowlerComs.h"
#include <Arduino.h>
#include <bowlerDeviceServerUtil.hpp>
#include <unity.h>

void discover_with_packetid_equal_to_discovery_packetid() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // Can't add a new packet with the discovery packet id
  std::array<std::uint8_t, DEFAULT_PAYLOAD_SIZE> buffer{OPERATION_DISCOVERY_ID,
                                                        DISCOVERY_PACKET_ID,
                                                        RESOURCE_TYPE_ANALOG_IN,
                                                        ATTACHMENT_POINT_TYPE_PIN,
                                                        1};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_REJECTED_INVALID_PACKET_ID, buffer[0]);
}

void discover_group_with_packetid_equal_to_discovery_packetid() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // Can't add a new group with the discovery packet id
  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{
    OPERATION_GROUP_DISCOVERY_ID, DISCOVERY_PACKET_ID, 1};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_REJECTED_INVALID_PACKET_ID, buffer[0]);
}

void discover_group_twice() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // The first time the group is discovered, it should be accepted
  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{OPERATION_GROUP_DISCOVERY_ID, 1, 2, 1};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_ACCEPTED, buffer[0]);

  // The second time the group is discovered, it should be rejected
  buffer = {OPERATION_GROUP_DISCOVERY_ID, 1, 3, 1};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_REJECTED_INVALID_GROUP_ID, buffer[0]);
}

void discover_group_with_zero_members() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // A group with a count of zero should be accepted
  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{OPERATION_GROUP_DISCOVERY_ID, 1, 2, 0};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_ACCEPTED, buffer[0]);
}

void discover_group_member_in_nonexistent_group() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // Can't put a group member into a nonexistent group
  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{OPERATION_GROUP_MEMBER_DISCOVERY_ID,
                                                       1,
                                                       0,
                                                       0,
                                                       0,
                                                       1,
                                                       RESOURCE_TYPE_DIGITAL_IN,
                                                       ATTACHMENT_POINT_TYPE_PIN,
                                                       2};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_REJECTED_INVALID_GROUP_ID, buffer[0]);
}

void add_too_many_members_to_group() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // Discover the group
  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{OPERATION_GROUP_DISCOVERY_ID, 1, 2, 1};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_ACCEPTED, buffer[0]);

  // Adding the first member is fine
  buffer = {OPERATION_GROUP_MEMBER_DISCOVERY_ID,
            1,
            0,
            0,
            0,
            1,
            RESOURCE_TYPE_DIGITAL_IN,
            ATTACHMENT_POINT_TYPE_PIN,
            2};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_ACCEPTED, buffer[0]);

  // Adding the second member is an error
  buffer = {OPERATION_GROUP_MEMBER_DISCOVERY_ID,
            1,
            0,
            0,
            1,
            2,
            RESOURCE_TYPE_DIGITAL_IN,
            ATTACHMENT_POINT_TYPE_PIN,
            4};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_REJECTED_GROUP_FULL, buffer[0]);
}

void add_group_member_with_send_end_greater_than_send_start() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // Discover the group
  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{OPERATION_GROUP_DISCOVERY_ID, 1, 2, 1};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_ACCEPTED, buffer[0]);

  // Adding the first member is fine
  buffer = {OPERATION_GROUP_MEMBER_DISCOVERY_ID,
            1,
            1,
            0,
            0,
            0,
            RESOURCE_TYPE_DIGITAL_OUT,
            ATTACHMENT_POINT_TYPE_PIN,
            2};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_REJECTED_GENERIC, buffer[0]);
}

void add_group_member_with_receive_end_greater_than_receive_start() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // Discover the group
  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{OPERATION_GROUP_DISCOVERY_ID, 1, 2, 1};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_ACCEPTED, buffer[0]);

  // Adding the first member is fine
  buffer = {OPERATION_GROUP_MEMBER_DISCOVERY_ID,
            1,
            0,
            0,
            1,
            0,
            RESOURCE_TYPE_DIGITAL_IN,
            ATTACHMENT_POINT_TYPE_PIN,
            2};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_REJECTED_GENERIC, buffer[0]);
}

void unknown_operation() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{99};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_REJECTED_UNKNOWN_OPERATION, buffer[0]);
}

void discard() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // Discover the group
  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{OPERATION_GROUP_DISCOVERY_ID, 1, 2, 1};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_ACCEPTED, buffer[0]);

  // Adding the first member is fine
  buffer = {OPERATION_GROUP_MEMBER_DISCOVERY_ID,
            1,
            0,
            0,
            0,
            1,
            RESOURCE_TYPE_DIGITAL_IN,
            ATTACHMENT_POINT_TYPE_PIN,
            2};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_ACCEPTED, buffer[0]);

  // Discover a resource
  buffer = {OPERATION_DISCOVERY_ID, 3, RESOURCE_TYPE_DIGITAL_IN, ATTACHMENT_POINT_TYPE_PIN, 18};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_ACCEPTED, buffer[0]);

  // Discard
  buffer = {OPERATION_DISCARD_DISCOVERY_ID};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_DISCARD_COMPLETE, buffer[0]);
}

void discard_with_empty_server() {
  auto coms = std::shared_ptr<MockBowlerComs>(new MockBowlerComs());
  DiscoveryPacket p(coms);

  // Discard
  std::array<std::uint8_t, DEFAULT_PACKET_SIZE> buffer{OPERATION_DISCARD_DISCOVERY_ID};
  p.event(buffer.data());
  TEST_ASSERT_EQUAL(STATUS_DISCARD_COMPLETE, buffer[0]);
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(discover_with_packetid_equal_to_discovery_packetid);
  RUN_TEST(discover_group_with_packetid_equal_to_discovery_packetid);
  RUN_TEST(discover_group_twice);
  RUN_TEST(discover_group_with_zero_members);
  RUN_TEST(discover_group_member_in_nonexistent_group);
  RUN_TEST(add_too_many_members_to_group);
  RUN_TEST(add_group_member_with_send_end_greater_than_send_start);
  RUN_TEST(add_group_member_with_receive_end_greater_than_receive_start);
  RUN_TEST(unknown_operation);
  RUN_TEST(discard);
  RUN_TEST(discard_with_empty_server);
  UNITY_END();
}

void loop() {
}
