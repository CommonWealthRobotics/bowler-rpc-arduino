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
#pragma once

#include "../resource/Resource.h"
#include <Esp32SimplePacketComs.h>
#include <SimplePacketComs.h>
#include <cstring>

class ResourceServer : public PacketEventAbstract {
  public:
  ResourceServer(std::uint8_t ipacketId, std::unique_ptr<Resource> iresource)
    : PacketEventAbstract(ipacketId), resource(std::move(iresource)) {
  }

  virtual ~ResourceServer() {
  }

  void event(float *buffer) override {
    std::uint8_t *buf = (std::uint8_t *)buffer;
    resource->readFromPayload(buf);
    std::memset(buf, 0, PAYLOAD_LENGTH * (sizeof buf[0]));
    resource->writeToPayload(buf);
  }

  protected:
  std::unique_ptr<Resource> resource;
};
