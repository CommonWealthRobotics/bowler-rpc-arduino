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
#include "DiscoveryPacket.h"
#include "../resource/AnalogInResource.h"
#include "../resource/DigitalOutResource.h"
#include <Arduino.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>

#if defined(PLATFORM_ESP32)
#include "../resource/ESP32ServoResource.h"
#elif defined(PLATFORM_TEENSY)
#include "../resource/TeensyServoResource.h"
#endif

// User function to be called when a packet comes in
// Buffer contains data from the packet coming in at the start of the function
// User data is written into the buffer to send it back
void DiscoveryPacket::event(float *buffer) {
  std::uint8_t *buf = (std::uint8_t *)buffer;

  // Print the bytes we got
  for (int i = 0; i < 60; i++) {
    Serial.print(buf[i]);
    Serial.print(", ");
  }
  Serial.println();

  parseGeneralDiscoveryPacket(buf);
}

void DiscoveryPacket::parseGeneralDiscoveryPacket(std::uint8_t *buffer) {
  const std::uint8_t operation = buffer[0];
  std::uint8_t *dest = (std::uint8_t *)std::calloc(PAYLOAD_LENGTH, sizeof(std::uint8_t));

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

  groupServers.emplace(groupId, new GroupResourceServer(packetId, count));

  coms->attach(groupServers.at(groupId));

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
    delete coms->detach(elem->getId());
  }
  resourceServers.clear();

  // Always detach before clearing
  for (auto const &elem : groupServers) {
    delete coms->detach(elem.second->getId());
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
  std::unique_ptr<Resource> resource;
  std::uint8_t status;
  std::tie(resource, status) = makeResource(resourceType, attachment, attachmentData);

  // Validation errors cause the `resource` to be `nullptr`. The `status` is always good.
  if (resource) {
    auto server = new ResourceServer(packetId, std::move(resource));
    resourceServers.push_back(server);
    coms->attach(server);
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
