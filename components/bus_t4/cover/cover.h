#pragma once

#include "esphome/components/cover/cover.h"
#include "../bus_t4.h"

namespace esphome::bus_t4 {

class BusT4Cover : public cover::Cover, public BusT4Device, public Component {
 public:
  BusT4Cover() = default;
  cover::CoverTraits get_traits() override;

//  void setup() override;
//  void loop() override;
//  void dump_config() override;
//  void set_parent(BusT4 *parent) { this->parent_ = parent; }

 protected:
  void control(const cover::CoverCall &call) override;
};

} // namespace esphome::bus_t4
