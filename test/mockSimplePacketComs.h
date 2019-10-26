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
#ifndef MOCKSIMPLEPACKETCOMS_H
#define MOCKSIMPLEPACKETCOMS_H

#include <SimplePacketComs.h>

namespace bowlerrpc {
/**
 * A mock SimplePacketComs that does not receive or send packets.
 */
class MockSimplePacketComs : public SimplePacketComsAbstract {
  bool isPacketAvailible() override {
    return false;
  }

  int32_t getPacket(uint8_t *buffer, uint32_t numberOfBytes) override {
    return 0;
  }

  int32_t sendPacket(uint8_t *buffer, uint32_t numberOfBytes) override {
    return 0;
  }
};
} // namespace bowlerrpc

#endif
