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
#ifndef MOCKBOWLERCOMS_H
#define MOCKBOWLERCOMS_H

#include <bowlerComs.hpp>
#include <bowlerDeviceServerUtil.hpp>

class MockBowlerServer : public BowlerServer<DEFAULT_PACKET_SIZE> {
  public:
  std::int32_t write(std::array<std::uint8_t, DEFAULT_PACKET_SIZE> ipayload) override {
    return 1;
  }

  std::int32_t read(std::array<std::uint8_t, DEFAULT_PACKET_SIZE> &ipayload) override {
    return 1;
  }

  std::int32_t isDataAvailable(bool &iavailable) override {
    iavailable = true;
    return 1;
  }
};

class MockBowlerComs : public BowlerComs<DEFAULT_PACKET_SIZE> {
  public:
  MockBowlerComs()
    : BowlerComs<DEFAULT_PACKET_SIZE>(std::unique_ptr<MockBowlerServer>(new MockBowlerServer())) {
  }
};

#endif
