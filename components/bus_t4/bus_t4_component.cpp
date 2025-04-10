#include "bus_t4_component.h"

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#include "t4_packet.h"

namespace esphome::bus_t4 {

static const char *TAG = "bus_t4";

void BusT4Component::setup() {
  rxQueue_ = xQueueCreate(32, sizeof(T4Packet));
  txQueue_ = xQueueCreate(32, sizeof(T4Packet));

  requestEvent_ = xEventGroupCreate();
  xEventGroupSetBits(requestEvent_, EB_REQUEST_FREE);

  xTaskCreate(rxTaskThunk, "bus_t4_rx", 8192, this, 10, &rxTask_);
  xTaskCreate(txTaskThunk, "bus_t4_tx", 8192, this, 10, &txTask_);
}

void BusT4Component::dump_config() {
  ESP_LOGCONFIG(TAG, "BusT4:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", address_.address);
  ESP_LOGCONFIG(TAG, "  Endpoint: 0x%02X", address_.endpoint);
}

void BusT4Component::rxTask() {
  T4Packet packet;
  uint8_t checksum = 0;
  enum { WAIT = 0, SYNC, SIZE, DATA, CHECKSUM, RESET } rx_state = WAIT;

  for (;;) {
    uint8_t byte;
    if (parent_->available() && parent_->read_byte(&byte) == true) {
      if (rx_state != WAIT && rx_state != SYNC)
        packet.data[packet.size++] = byte;

      switch (rx_state) {
        case WAIT:
          rx_state = (byte == T4_BREAK) ? SYNC : RESET;
          break;

        case SYNC:
          rx_state = (byte == T4_SYNC) ? SIZE : RESET;
          break;

        case SIZE:
          rx_state = (byte <= 60) ? DATA : RESET;
          break;

        case DATA:
          checksum ^= byte;
          if (packet.size == packet.data[1] + 1)
            rx_state = CHECKSUM;
          break;

        case CHECKSUM:
          if (byte == checksum)
            ESP_LOGD(TAG, "Received packet: %s", format_hex_pretty(packet.data, packet.size).c_str());
            xQueueSend(rxQueue_, &packet, portMAX_DELAY);
          rx_state = RESET;
          break;

        case RESET:
          packet.size = 0;
          checksum = 0;
          rx_state = WAIT;
          break;
      }
    } else {
      rx_state = RESET;
    }
    vTaskDelay(2);
  }

  rxTask_ = nullptr;
  vTaskDelete(nullptr);
}

void BusT4Component::txTask() {
  for (;;) {
    T4Packet packet;

    if (xQueueReceive(txQueue_, &packet, 0)) {
      ESP_LOGD(TAG, "Sending packet: %s", format_hex_pretty(packet.data, packet.size).c_str());
      parent_->write_byte(T4_BREAK);
      parent_->write_byte(T4_SYNC);
      parent_->write_byte(packet.size);
      parent_->write_array(packet.data, packet.size);
      parent_->write_byte(packet.size);
      parent_->flush();
    }
    vTaskDelay(2);
  }

  txTask_ = nullptr;
  vTaskDelete(nullptr);
}

} // namespace esphome::bus_t4