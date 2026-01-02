#pragma once

#include <vector>
#include <functional>

#include <stdint.h>

namespace esphome {
namespace dynamixel {
namespace protocol_v2 {

struct StatusPacket {
  uint8_t id{0};
  uint16_t error{0};
  std::vector<uint8_t> parameters;
};

using ReadFn = std::function<bool(uint8_t *data, size_t len, uint32_t timeout_ms)>;

std::vector<uint8_t> build_ping(uint8_t id);
std::vector<uint8_t> build_read(uint8_t id, uint16_t address, uint16_t length);
std::vector<uint8_t> build_write(uint8_t id, uint16_t address, const std::vector<uint8_t> &data);
std::vector<uint8_t> build_reboot(uint8_t id);

bool read_status(const ReadFn &read_fn, StatusPacket &status, uint32_t timeout_ms);

}  // namespace protocol_v2
}  // namespace dynamixel
}  // namespace esphome
