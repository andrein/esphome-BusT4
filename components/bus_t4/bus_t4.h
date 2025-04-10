#pragma once

#include "bus_t4_component.h"

#include <freertos/queue.h>
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

static const char *TAG = "bus_t4.device";

namespace esphome::bus_t4 {

class BusT4Device {
public:
  BusT4Device() = default;
  explicit BusT4Device(BusT4Component *parent) : parent_(parent) {}

  bool read(T4Packet *packet, TickType_t xTicksToWait) {
    return parent_->read(packet, xTicksToWait);
  }

  bool write(T4Packet *packet, TickType_t xTicksToWait) {
    return parent_->write(packet, xTicksToWait);
  }

  void set_parent(BusT4Component *parent) { this->parent_ = parent; }
protected:
  BusT4Component *parent_{nullptr};
};

} // namespace esphome::bus_t4
