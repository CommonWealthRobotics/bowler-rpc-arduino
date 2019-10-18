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
#ifndef DIGITALOUTRESOURCE_H
#define DIGITALOUTRESOURCE_H

#include "commands/discoveryMetadata.h"
#include "resource/resource.h"
#include <Arduino.h>
#include <cstring>

class DigitalOutResource : public Resource {
  public:
  std::uint8_t initialize(std::uint8_t resource,
                          std::uint8_t attachment,
                          const std::uint8_t *attachmentData) override {
    pin = attachmentData[0];
    pinMode(pin, OUTPUT);
    return STATUS_ACCEPTED;
  }

  void readFromPayload(std::uint8_t *buffer) override {
    digitalWrite(pin, buffer[0]);
  }

  void writeToPayload(std::uint8_t *buffer) override {
  }

  protected:
  std::uint8_t pin;
};

/**
 * Checks if the attachment data is valid for the resource implementation.
 *
 * @param attachmentData The attachment data to validate.
 * @return The status code; `STATUS_ACCEPTED` if the attachment data is valid.
 */
static std::uint8_t validateDigitalOutAttachmentData(const std::uint8_t *attachmentData) {
  std::uint8_t pin = attachmentData[0];

#if defined(PLATFORM_ESP32)
  if (pin == 2 || pin == 4 || pin == 5 || (pin >= 12 && pin <= 19) || (pin >= 21 && pin <= 23) ||
      (pin >= 25 && pin <= 27) || pin == 32 || pin == 33) {
    return STATUS_ACCEPTED;
  } else {
    return STATUS_REJECTED_INVALID_ATTACHMENT_DATA;
  }
#elif defined(PLATFORM_TEENSY)
  if (pin >= 0 && pin <= 57) {
    return STATUS_ACCEPTED;
  } else {
    return STATUS_REJECTED_INVALID_ATTACHMENT_DATA;
  }
#endif
}
#endif
