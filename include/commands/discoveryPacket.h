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

#include "resource/resource.h"
#include "commands/discoveryMetadata.h"
#include "commands/groupResourceServer.h"
#include "commands/resourceServer.h"
#include <SimplePacketComs.h>
#include <array>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

/**
 * Handles validating a resource type by name. If the resource type is valid, a new resource is
 * created and returned. Else, an error is returned.
 *
 * Validation is done using a method named `validate##RESOURCE_TYPE_NAME##AttachmentData`. The
 * resource class must be named `RESOURCE_TYPE_NAME##Resource`.
 */
#define VALIDATE_AND_RETURN(RESOURCE_TYPE_NAME)                                                    \
  std::uint8_t validationStatus = validate##RESOURCE_TYPE_NAME##AttachmentData(attachmentData);    \
  if (validationStatus != STATUS_ACCEPTED) {                                                       \
    return std::make_tuple(nullptr, STATUS_REJECTED_UNKNOWN_ATTACHMENT);                           \
  } else {                                                                                         \
    return std::make_tuple(                                                                        \
      std::unique_ptr<RESOURCE_TYPE_NAME##Resource>(                                               \
        new RESOURCE_TYPE_NAME##Resource(resourceType, attachment, attachmentData)),               \
      STATUS_ACCEPTED);                                                                            \
  }

/**
 * Handles the case of an unknown attachment (by returning an error).
 */
#define CASE_UNKNOWN_ATTACHMENT                                                                    \
  default: { return std::make_tuple(nullptr, STATUS_REJECTED_UNKNOWN_ATTACHMENT); }

class DiscoveryPacket : public PacketEventAbstract {
  public:
  DiscoveryPacket(SimplePacketComsAbstract *icoms)
    : PacketEventAbstract(DISCOVERY_PACKET_ID), coms(icoms) {
  }

  virtual ~DiscoveryPacket() {
  }

  // User function to be called when a packet comes in
  // Buffer contains data from the packet coming in at the start of the function
  // User data is written into the buffer to send it back
  void event(float *buffer) override;

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
   * Attaches a resource.
   *
   * @param packetId The new packet ID.
   * @param resourceType The resource type.
   * @param attachment The attachment point type.
   * @param attachmentData Any attachment data.
   * @param dest The destination array to write the status and payload to.
   */
  virtual void attachResource(std::uint8_t packetId,
                              std::uint8_t resourceType,
                              std::uint8_t attachment,
                              const std::uint8_t *attachmentData,
                              std::uint8_t *dest);

  /**
   * Makes a resource.
   *
   * @param resourceType The resource type.
   * @param attachment The attachment point type.
   * @param attachmentData Any attachment data.
   * @return The resource and a status code.
   */
  virtual std::tuple<std::unique_ptr<Resource>, std::uint8_t>
  makeResource(std::uint8_t resourceType,
               std::uint8_t attachment,
               const std::uint8_t *attachmentData);

  SimplePacketComsAbstract *coms;

  // All the attached resource servers
  std::vector<ResourceServer *> resourceServers{};

  // Keys are group id's (not packet id's), values are attached group resource servers
  std::map<std::uint8_t, GroupResourceServer *> groupServers{};
};

#endif
