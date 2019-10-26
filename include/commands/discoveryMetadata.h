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
#ifndef DISCOVERYMETADATA_H
#define DISCOVERYMETADATA_H

#include <cstdint>

namespace bowlerrpc {
const std::uint8_t DISCOVERY_PACKET_ID = 2;
const std::uint32_t PAYLOAD_LENGTH = 61;
const std::uint32_t ATTACHMENT_DATA_LENGTH = 56;

const std::uint8_t OPERATION_DISCOVERY_ID = 1;
const std::uint8_t OPERATION_GROUP_DISCOVERY_ID = 2;
const std::uint8_t OPERATION_GROUP_MEMBER_DISCOVERY_ID = 3;
const std::uint8_t OPERATION_DISCARD_DISCOVERY_ID = 4;

const std::uint8_t STATUS_ACCEPTED = 1;
const std::uint8_t STATUS_REJECTED_GENERIC = 2;
const std::uint8_t STATUS_REJECTED_UNKNOWN_RESOURCE = 3;
const std::uint8_t STATUS_REJECTED_UNKNOWN_ATTACHMENT = 4;
const std::uint8_t STATUS_REJECTED_INVALID_ATTACHMENT = 5;
const std::uint8_t STATUS_REJECTED_INVALID_ATTACHMENT_DATA = 6;
const std::uint8_t STATUS_REJECTED_INVALID_GROUP_ID = 7;
const std::uint8_t STATUS_REJECTED_GROUP_FULL = 8;
const std::uint8_t STATUS_REJECTED_UNKNOWN_OPERATION = 9;
const std::uint8_t STATUS_DISCARD_IN_PROGRESS = 10;
const std::uint8_t STATUS_DISCARD_COMPLETE = 11;
const std::uint8_t STATUS_REJECTED_INVALID_PACKET_ID = 12;

const std::uint8_t RESOURCE_TYPE_DIGITAL_IN = 1;
const std::uint8_t RESOURCE_TYPE_DIGITAL_OUT = 2;
const std::uint8_t RESOURCE_TYPE_ANALOG_IN = 3;
const std::uint8_t RESOURCE_TYPE_ANALOG_OUT = 4;
const std::uint8_t RESOURCE_TYPE_SERIAL_CONNECTION = 5;
const std::uint8_t RESOURCE_TYPE_SERVO = 6;
const std::uint8_t RESOURCE_TYPE_STEPPER = 7;
const std::uint8_t RESOURCE_TYPE_ENCODER = 8;
const std::uint8_t RESOURCE_TYPE_BUTTON = 9;
const std::uint8_t RESOURCE_TYPE_ULTRASONIC = 10;
const std::uint8_t RESOURCE_TYPE_PIEZO = 11;

const std::uint8_t ATTACHMENT_POINT_TYPE_PIN = 1;
const std::uint8_t ATTACHMENT_POINT_TYPE_PIN_GROUP = 2;
const std::uint8_t ATTACHMENT_POINT_TYPE_USB_PORT = 3;
const std::uint8_t ATTACHMENT_POINT_TYPE_PWM_PIN = 4;
const std::uint8_t ATTACHMENT_POINT_TYPE_NONE = 5;
} // namespace bowlerrpc

#endif
