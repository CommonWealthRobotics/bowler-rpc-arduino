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
#ifndef DISCOVERYPACKET_H
#define DISCOVERYPACKET_H

#include "commands/discoveryMetadata.h"
#include "commands/groupResourceServer.h"
#include "commands/resourceServer.h"
#include "resource/resource.h"
#include <array>
#include <bowlerComs.hpp>
#include <bowlerDeviceServerUtil.hpp>
#include <bowlerPacket.hpp>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

/**
 * Handles validating a resource type by name. If the resource type is valid, a new resource is
 * created, initialized, and returned. Else, an error is returned.
 *
 * Validation is done using a method named `validate##RESOURCE_TYPE_NAME##AttachmentData`. The
 * resource class must be named `RESOURCE_TYPE_NAME##Resource`.
 */
#define VALIDATE_AND_RETURN(RESOURCE_TYPE_NAME)                                                    \
  std::uint8_t validationStatus = validate##RESOURCE_TYPE_NAME##AttachmentData(attachmentData);    \
  if (validationStatus != bowlerrpc::STATUS_ACCEPTED) {                                            \
    return std::make_tuple(nullptr, validationStatus);                                             \
  } else {                                                                                         \
    auto resource =                                                                                \
      std::unique_ptr<RESOURCE_TYPE_NAME##Resource>(new RESOURCE_TYPE_NAME##Resource(isReliable)); \
    std::uint8_t initializeStatus =                                                                \
      resource->initialize(resourceType, attachment, attachmentData);                              \
    if (initializeStatus == bowlerrpc::STATUS_ACCEPTED) {                                          \
      return std::make_tuple(std::move(resource), initializeStatus);                               \
    } else {                                                                                       \
      return std::make_tuple(nullptr, initializeStatus);                                           \
    }                                                                                              \
  }

/**
 * Handles the case of an invalid attachment (by returning an error).
 */
#define CASE_INVALID_ATTACHMENT                                                                    \
  default: { return std::make_tuple(nullptr, bowlerrpc::STATUS_REJECTED_INVALID_ATTACHMENT); }

namespace bowlerrpc {
class DiscoveryPacket : public bowlerserver::Packet {
  public:
  DiscoveryPacket(
    std::shared_ptr<bowlerserver::BowlerComs<bowlerserver::DEFAULT_PACKET_SIZE>> icoms)
    : Packet(DISCOVERY_PACKET_ID, true), coms(std::move(icoms)) {
  }

  std::int32_t event(std::uint8_t *payload) override;

  protected:
  /**
   * Parses a general discovery packet and writes the response.
   *
   * Section 2.1.1.
   *
   * @param buffer The packet payload.
   */
  void parseGeneralDiscoveryPacket(std::uint8_t *buffer);

  /**
   * Parses a discovery packet and writes the response.
   *
   * Section 2.1.2.
   *
   * @param buffer The packet payload.
   * @param dest The reply buffer.
   */
  void parseDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest);

  /**
   * Parses a group discovery packet and writes the response.
   *
   * Section 2.1.3.
   *
   * @param buffer The packet payload.
   * @param dest The reply buffer.
   */
  void parseGroupDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest);

  /**
   * Parses a group member discovery packet and writes the response.
   *
   * Section 2.1.4.
   *
   * @param buffer The packet payload.
   * @param dest The reply buffer.
   */
  void parseGroupMemberDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest);

  /**
   * Parses a discard discovery packet and writes the response.
   *
   * Section 2.1.5.
   *
   * @param buffer The packet payload.
   * @param dest The reply buffer.
   */
  void parseDiscardDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest);

  /**
   * Makes a resource.
   *
   * @param resourceType The resource type.
   * @param attachment The attachment point type.
   * @param isReliable Whether this resource uses reliable transport.
   * @param attachmentData Any attachment data.
   * @return The resource and a status code.
   */
  virtual std::tuple<std::unique_ptr<Resource>, std::uint8_t>
  makeResource(std::uint8_t resourceType,
               std::uint8_t attachment,
               bool isReliable,
               const std::uint8_t *attachmentData);

  std::shared_ptr<bowlerserver::BowlerComs<bowlerserver::DEFAULT_PACKET_SIZE>> coms;

  // All the attached resource servers
  std::vector<std::shared_ptr<ResourceServer>> resourceServers{};

  // Keys are group id's (not packet id's), values are attached group resource servers
  std::map<std::uint8_t, std::shared_ptr<GroupResourceServer>> groupServers{};
};
} // namespace bowlerrpc

#endif
