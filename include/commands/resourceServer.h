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
#ifndef RESOURCESERVER_H
#define RESOURCESERVER_H

#include "../resource/resource.h"
#include <bowlerDeviceServerUtil.hpp>
#include <bowlerPacket.hpp>
#include <cstring>
#include <memory>

namespace bowler {
class ResourceServer : public Packet {
  public:
  ResourceServer(std::uint8_t ipacketId, std::unique_ptr<Resource> iresource)
    : Packet(ipacketId, false), resource(std::move(iresource)) {
  }

  virtual ~ResourceServer() {
  }

  std::int32_t event(std::uint8_t *payload) override {
    resource->readFromPayload(payload);
    std::memset(payload, 0, PAYLOAD_LENGTH * (sizeof payload[0]));
    resource->writeToPayload(payload);
    return 1;
  }

  protected:
  std::unique_ptr<Resource> resource;
};
} // namespace bowler

#endif
