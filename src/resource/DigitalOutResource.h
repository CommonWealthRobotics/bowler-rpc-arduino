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

class DigitalOutResource : public Resource {
  public:
  DigitalOutResource(std::uint8_t resource,
                     std::uint8_t attachment,
                     const std::uint8_t *attachmentData)
    : Resource(resource, attachment, attachmentData), pin(attachmentData[0]) {
    pinMode(pin, OUTPUT);
  }

  virtual ~DigitalOutResource() {
  }

  void readFromPayload(std::uint8_t *buffer) override {
    digitalWrite(pin, buffer[0]);
  }

  void writeToPayload(std::uint8_t *buffer) override {
  }

  protected:
  std::uint8_t pin;
};
