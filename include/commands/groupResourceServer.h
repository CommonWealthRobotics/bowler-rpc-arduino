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
#ifndef GROUPRESOURCESERVER_H
#define GROUPRESOURCESERVER_H

#include "../resource/resource.h"
#include <SimplePacketComs.h>
#include <cstring>
#include <memory>
#include <vector>

class GroupResourceServer : public PacketEventAbstract {
  public:
  GroupResourceServer(std::uint8_t ipacketId, std::uint8_t inumOfMembers)
    : PacketEventAbstract(ipacketId), spaceRemaining(inumOfMembers) {
    resources.reserve(spaceRemaining);
  }

  virtual ~GroupResourceServer() {
  }

  void event(float *buffer) override {
    std::uint8_t *buf = (std::uint8_t *)buffer;

    for (auto &resource : resources) {
      resource->readFromPayload(buf);
      buf = buf + resource->getReceivePayloadLength();
    }

    buf = (std::uint8_t *)buffer;
    std::memset(buf, 0, PAYLOAD_LENGTH * (sizeof buf[0]));

    for (auto &resource : resources) {
      resource->writeToPayload(buf);
      buf = buf + resource->getSendPayloadLength();
    }
  }

  void addResource(std::unique_ptr<Resource> resource) {
    resources.push_back(std::move(resource));
    spaceRemaining--;
  }

  bool hasSpaceRemaining() const {
    return spaceRemaining > 0;
  }

  protected:
  std::vector<std::unique_ptr<Resource>> resources{};
  std::uint8_t spaceRemaining{0};
};

#endif
