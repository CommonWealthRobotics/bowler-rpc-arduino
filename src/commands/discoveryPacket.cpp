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
#if defined(USE_WIFI)
#include <WiFi.h> // This is needed so the PlatformIO finds the right WiFi implementation
#endif

#include "commands/discoveryPacket.h"
#include "resource/analogInResource.h"
#include "resource/digitalInResource.h"
#include "resource/digitalOutResource.h"
#include <Arduino.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>

#if defined(PLATFORM_ESP32)
#include "resource/esp32ServoResource.h"
#elif defined(PLATFORM_TEENSY)
#include "resource/teensyServoResource.h"
#endif

namespace bowlerrpc {
std::int32_t DiscoveryPacket::event(std::uint8_t *payload) {
#if defined(DEBUG_DISCOVERY)
  Serial.println("Got ");
  for (int i = 0; i < PAYLOAD_LENGTH; i++) {
    Serial.print(payload[i]);
    Serial.print(", ");
  }
  Serial.println();
#endif

  parseGeneralDiscoveryPacket(payload);

#if defined(DEBUG_DISCOVERY)
  Serial.println("Sending ");
  for (int i = 0; i < PAYLOAD_LENGTH; i++) {
    Serial.print(payload[i]);
    Serial.print(", ");
  }
  Serial.println();
#endif

  return 1;
}

void DiscoveryPacket::parseGeneralDiscoveryPacket(std::uint8_t *buffer) {
  const std::uint8_t operation = buffer[0];
  std::uint8_t *dest = (std::uint8_t *)std::calloc(PAYLOAD_LENGTH, sizeof(buffer[0]));

  if (dest == nullptr) {
    // Can't allocate another buffer so we have to reuse the original buffer to send the error
    std::memset(buffer, 0, PAYLOAD_LENGTH * sizeof(buffer[0]));
    buffer[0] = STATUS_REJECTED_GENERIC;
    return;
  }

  switch (operation) {
  case OPERATION_DISCOVERY_ID:
    parseDiscoveryPacket(buffer, dest);
    break;

  case OPERATION_GROUP_DISCOVERY_ID:
    parseGroupDiscoveryPacket(buffer, dest);
    break;

  case OPERATION_GROUP_MEMBER_DISCOVERY_ID:
    parseGroupMemberDiscoveryPacket(buffer, dest);
    break;

  case OPERATION_DISCARD_DISCOVERY_ID:
    parseDiscardDiscoveryPacket(buffer, dest);
    break;

  // Operations that haven't already been handled are unknown
  default:
    dest[0] = STATUS_REJECTED_UNKNOWN_OPERATION;
    break;
  }

  std::memcpy(buffer, dest, PAYLOAD_LENGTH * sizeof(buffer[0]));
}

void DiscoveryPacket::parseDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest) {
  std::uint8_t packetId = buffer[1];
  std::uint8_t resourceType = buffer[2];
  std::uint8_t attachment = buffer[3];
  const std::uint8_t *attachmentData = buffer + 4;

  attachResource(packetId, resourceType, attachment, attachmentData, dest);
}

void DiscoveryPacket::parseGroupDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest) {
  std::uint8_t groupId = buffer[1];
  std::uint8_t packetId = buffer[2];
  std::uint8_t count = buffer[3];

  if (groupServers.find(groupId) != groupServers.end()) {
    // Check if the group was already created
    dest[0] = STATUS_REJECTED_INVALID_GROUP_ID;
    return;
  } else if (packetId == DISCOVERY_PACKET_ID) {
    // Can't have a packet id equal to the discovery packet id
    dest[0] = STATUS_REJECTED_INVALID_PACKET_ID;
    return;
  }

  std::shared_ptr<GroupResourceServer> server(new GroupResourceServer(packetId, count));
  groupServers[groupId] = server;
  coms->addPacket(server);
  dest[0] = STATUS_ACCEPTED;
}

void DiscoveryPacket::parseGroupMemberDiscoveryPacket(const std::uint8_t *buffer,
                                                      std::uint8_t *dest) {
  std::uint8_t groupId = buffer[1];
  std::uint8_t sendStart = buffer[2];
  std::uint8_t sendEnd = buffer[3];
  std::uint8_t receiveStart = buffer[4];
  std::uint8_t receiveEnd = buffer[5];
  std::uint8_t resourceType = buffer[6];
  std::uint8_t attachment = buffer[7];
  const std::uint8_t *attachmentData = buffer + 8;

  if (groupServers.find(groupId) == groupServers.end()) {
    // Check that the group exists first
    dest[0] = STATUS_REJECTED_INVALID_GROUP_ID;
    return;
  } else if (!groupServers.at(groupId)->hasSpaceRemaining()) {
    // Check if the resource can be added to the group before we make it
    dest[0] = STATUS_REJECTED_GROUP_FULL;
    return;
  } else if (sendStart > sendEnd || receiveStart > receiveEnd) {
    // Check if the send or receive indices are invalid
    dest[0] = STATUS_REJECTED_GENERIC;
    return;
  }

  std::unique_ptr<Resource> resource;
  std::uint8_t status;
  std::tie(resource, status) = makeResource(resourceType, attachment, attachmentData);

  // Validation errors cause the `resource` to be `nullptr`. The `status` is always good.
  if (resource) {
    // Send length is from the PC perspective, which is our receive length
    resource->setReceivePayloadLength(sendEnd - sendStart);

    // Receive length is from the PC perspective, which is our send length
    resource->setSendPayloadLength(receiveEnd - receiveStart);

    groupServers.at(groupId)->addResource(std::move(resource));
  }

  dest[0] = status;
}

void DiscoveryPacket::parseDiscardDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest) {
  // Always detach before clearing
  for (auto const &elem : resourceServers) {
    coms->removePacket(elem->getId());
  }
  resourceServers.clear();

  // Always detach before clearing
  for (auto const &elem : groupServers) {
    coms->removePacket(elem.second->getId());
  }
  groupServers.clear();

  // Nothing else to do
  dest[0] = STATUS_DISCARD_COMPLETE;
}

void DiscoveryPacket::attachResource(std::uint8_t packetId,
                                     std::uint8_t resourceType,
                                     std::uint8_t attachment,
                                     const std::uint8_t *attachmentData,
                                     std::uint8_t *dest) {
  if (packetId == DISCOVERY_PACKET_ID) {
    dest[0] = STATUS_REJECTED_INVALID_PACKET_ID;
    return;
  }

  std::unique_ptr<Resource> resource;
  std::uint8_t status;
  std::tie(resource, status) = makeResource(resourceType, attachment, attachmentData);

  // Validation errors cause the `resource` to be `nullptr`. The `status` is always good.
  if (resource) {
    std::shared_ptr<ResourceServer> server(new ResourceServer(packetId, std::move(resource)));
    resourceServers.push_back(server);
    coms->addPacket(server);
  }

  dest[0] = status;
}

std::tuple<std::unique_ptr<Resource>, std::uint8_t>
DiscoveryPacket::makeResource(std::uint8_t resourceType,
                              std::uint8_t attachment,
                              const std::uint8_t *attachmentData) {
  switch (resourceType) {
  case RESOURCE_TYPE_ANALOG_IN: {
    switch (attachment) {
    case ATTACHMENT_POINT_TYPE_PIN: {
      VALIDATE_AND_RETURN(AnalogIn)
    }

      CASE_UNKNOWN_ATTACHMENT
    }
  }

  case RESOURCE_TYPE_DIGITAL_IN: {
    switch (attachment) {
    case ATTACHMENT_POINT_TYPE_PIN: {
      VALIDATE_AND_RETURN(DigitalIn)
    }

      CASE_UNKNOWN_ATTACHMENT
    }
  }

  case RESOURCE_TYPE_DIGITAL_OUT: {
    switch (attachment) {
    case ATTACHMENT_POINT_TYPE_PIN: {
      VALIDATE_AND_RETURN(DigitalOut)
    }

      CASE_UNKNOWN_ATTACHMENT
    }
  }

  case RESOURCE_TYPE_SERVO: {
    switch (attachment) {
    case ATTACHMENT_POINT_TYPE_PIN:
    case ATTACHMENT_POINT_TYPE_PWM_PIN: {
      VALIDATE_AND_RETURN(Servo)
    }

      CASE_UNKNOWN_ATTACHMENT
    }
  }

  // Resources that haven't already been handled are unknown
  default: { return std::make_tuple(nullptr, STATUS_REJECTED_UNKNOWN_RESOURCE); }
  }
}
} // namespace bowlerrpc
