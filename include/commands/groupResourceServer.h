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
#include <bowlerDeviceServerUtil.hpp>
#include <bowlerPacket.hpp>
#include <cstring>
#include <memory>
#include <vector>

namespace bowlerrpc {
class GroupResourceServer : public bowlerserver::Packet {
  public:
  GroupResourceServer(std::uint8_t ipacketId, std::uint8_t inumOfMembers, bool iisReliable)
    : Packet(ipacketId, iisReliable), spaceRemaining(inumOfMembers) {
    resources.reserve(spaceRemaining);
  }

  virtual ~GroupResourceServer() {
  }

  std::int32_t event(std::uint8_t *payload) override {
    std::uint8_t *buf = payload;

    for (auto &resource : resources) {
      resource->readFromPayload(buf);
      buf = buf + resource->getReceivePayloadLength();
    }

    buf = payload;
    std::memset(buf, 0, PAYLOAD_LENGTH * (sizeof buf[0]));

    for (auto &resource : resources) {
      resource->writeToPayload(buf);
      buf = buf + resource->getSendPayloadLength();
    }

    return 1;
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
} // namespace bowlerrpc

#endif
