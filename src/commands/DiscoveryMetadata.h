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

#define DISCOVERY_PACKET_ID 1
#define PAYLOAD_LENGTH 60
#define ATTACHMENT_DATA_LENGTH 56

#define OPERATION_DISCOVERY_ID 1
#define OPERATION_GROUP_DISCOVERY_ID 2
#define OPERATION_GROUP_MEMBER_DISCOVERY_ID 3
#define OPERATION_DISCARD_DISCOVERY_ID 4

#define STATUS_ACCEPTED 1
#define STATUS_REJECTED_GENERIC 2
#define STATUS_REJECTED_UNKNOWN_RESOURCE 3
#define STATUS_REJECTED_UNKNOWN_ATTACHMENT 4
#define STATUS_REJECTED_INVALID_ATTACHMENT 5
#define STATUS_REJECTED_INVALID_ATTACHMENT_DATA 6
#define STATUS_REJECTED_INVALID_GROUP_ID 7
#define STATUS_REJECTED_GROUP_FULL 8
#define STATUS_REJECTED_UNKNOWN_OPERATION 9
#define STATUS_DISCARD_IN_PROGRESS 10
#define STATUS_DISCARD_COMPLETE 11

#define RESOURCE_TYPE_DIGITAL_IN 1
#define RESOURCE_TYPE_DIGITAL_OUT 2
#define RESOURCE_TYPE_ANALOG_IN 3
#define RESOURCE_TYPE_ANALOG_OUT 4
#define RESOURCE_TYPE_SERIAL_CONNECTION 5
#define RESOURCE_TYPE_SERVO 6
#define RESOURCE_TYPE_STEPPER 7
#define RESOURCE_TYPE_ENCODER 8
#define RESOURCE_TYPE_BUTTON 9
#define RESOURCE_TYPE_ULTRASONIC 10
#define RESOURCE_TYPE_PIEZO 11

#define ATTACHMENT_POINT_TYPE_PIN 1
#define ATTACHMENT_POINT_TYPE_PIN_GROUP 2
#define ATTACHMENT_POINT_TYPE_USB_PORT 3
