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

#include "../commands/DiscoveryMetadata.h"
#include "Resource.h"
#include <Arduino.h>
#include <cstring>

class AnalogInResource : public Resource {
  public:
  AnalogInResource(std::uint8_t resource,
                   std::uint8_t attachment,
                   const std::uint8_t *attachmentData)
    : Resource(resource, attachment, attachmentData), pin(attachmentData[0]) {
    pinMode(pin, INPUT);
  }

  virtual ~AnalogInResource() {
  }

  void readFromPayload(std::uint8_t *buffer) override {
  }

  void writeToPayload(std::uint8_t *buffer) override {
    std::int16_t value = analogRead(pin);
    buffer[0] = value & 0xF;
    buffer[1] = value & 0xF0;
  }

  protected:
  std::uint8_t pin;
};
