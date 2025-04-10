/*
   https://github.com/gashtaan/nice-bidiwifi-firmware

   Copyright (C) 2024, Michal Kovacik
   Copyright (C) 2025, Andrei Nistor

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 3, as
   published by the Free Software Foundation.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <algorithm>
#include <cstdint>

static constexpr uint8_t T4_BREAK = 0x00;
static constexpr uint8_t T4_SYNC = 0x55;

struct T4Source
{
  uint8_t address;
  uint8_t endpoint;

  bool operator==(const T4Source& other) const { return address == other.address && endpoint == other.endpoint; }
};

enum T4Protocol : uint8_t {
  DEP = 1,
  DMP = 8,
};

enum T4Device : uint8_t
{
  STANDARD = 0,
  OVIEW = 1,
  CONTROLLER = 4,
  SCREEN = 6,
  RADIO = 10,
};

struct T4Packet {
  uint8_t size = 0;
  union {
    uint8_t data[63];
    struct {
      struct {
        T4Source to;
        T4Source from;
        T4Protocol protocol;
        uint8_t messageSize;
        uint8_t checksum;
      } header;
      struct {
        uint8_t device;
        uint8_t command;
        union {
          struct {
            uint8_t flags;
            uint8_t sequence;
            uint8_t status;
            uint8_t data[0];
          } dmp;
          struct {
            uint8_t data[0];
          } dep;
        };
        uint8_t checksum;
      } message;
    };
  };

  uint8_t checksum(uint8_t i, uint8_t c) const
  {
    uint8_t h = 0;
    while (c-- > 0)
      h ^= data[i++];
    return h;
  }

  T4Packet() = default;
  T4Packet(const T4Source to, const T4Source from, const T4Protocol protocol, uint8_t *messageData,
           const uint8_t messageSize) {
    size = sizeof(header) + messageSize + 1;

    header.to = to;
    header.from = from;
    header.protocol = protocol;
    header.messageSize = messageSize + 1;
    header.checksum = checksum(0, sizeof(header) - 1);

    std::copy_n(messageData, messageSize, data + sizeof(header));

    data[size - 1] = checksum(sizeof(header), messageSize);
  }
};

enum T4Command : uint8_t {
  CMD_STEP = 0x01,
  CMD_STOP = 0x02,
  CMD_OPEN = 0x03,
  CMD_CLOSE = 0x04,
};