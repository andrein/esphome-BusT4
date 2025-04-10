#include "cover.h"

namespace esphome {
namespace bus_t4 {

static const char *TAG = "bus_t4.cover";

cover::CoverTraits BusT4Cover::get_traits() {
  auto traits = cover::CoverTraits();
  traits.set_is_assumed_state(true);
  traits.set_supports_position(false);
  traits.set_supports_tilt(false);
  traits.set_supports_toggle(false);
  traits.set_supports_stop(true);
  return traits;
}

void BusT4Cover::control(const cover::CoverCall &call) {
  if (call.get_position() == cover::COVER_OPEN) {
    ESP_LOGD(TAG, "Opening cover");
    const T4Source dst = T4Source{0x00, 0x03};
    uint8_t message[4] = { OVIEW, 0x82, CMD_OPEN, 0x64 };
    T4Packet packet(dst, parent_->get_address(), DEP, message, sizeof(message));
    write(&packet, 0);
    return;
  }

  if (call.get_position() == cover::COVER_CLOSED) {
    ESP_LOGD(TAG, "Closing cover");
    const T4Source dst = T4Source{0x00, 0x03};
    uint8_t message[4] = { OVIEW, 0x82, CMD_CLOSE, 0x64 };
    T4Packet packet(dst, parent_->get_address(), DEP, message, sizeof(message));
    write(&packet, 0);
    return;
  }

  if (call.get_stop()) {
    ESP_LOGD(TAG, "Stopping cover");
    const T4Source dst = T4Source{0x00, 0x03};
    uint8_t message[4] = { OVIEW, 0x82, CMD_STOP, 0x64 };
    T4Packet packet(dst, parent_->get_address(), DEP, message, sizeof(message));
    write(&packet,0 );
    return;
  }
}

} // bus_t4
} // esphome