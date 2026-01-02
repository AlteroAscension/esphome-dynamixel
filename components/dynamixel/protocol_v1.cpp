#include "protocol_v1.h"

namespace esphome {
namespace dynamixel {
namespace protocol_v1 {

static uint8_t checksum_(const std::vector<uint8_t> &packet) {
  uint16_t sum = 0;
  for (size_t i = 2; i < packet.size() - 1; i++) {
    sum += packet[i];
  }
  return static_cast<uint8_t>(~(sum & 0xFF));
}

static std::vector<uint8_t> build_packet_(uint8_t id, uint8_t instruction,
                                          const std::vector<uint8_t> &params) {
  std::vector<uint8_t> packet;
  packet.reserve(6 + params.size());
  packet.push_back(0xFF);
  packet.push_back(0xFF);
  packet.push_back(id);
  packet.push_back(static_cast<uint8_t>(params.size() + 2));
  packet.push_back(instruction);
  packet.insert(packet.end(), params.begin(), params.end());
  packet.push_back(0x00);
  packet.back() = checksum_(packet);
  return packet;
}

std::vector<uint8_t> build_ping(uint8_t id) { return build_packet_(id, 0x01, {}); }

std::vector<uint8_t> build_read(uint8_t id, uint16_t address, uint16_t length) {
  std::vector<uint8_t> params{static_cast<uint8_t>(address),
                              static_cast<uint8_t>(length)};
  return build_packet_(id, 0x02, params);
}

std::vector<uint8_t> build_write(uint8_t id, uint16_t address, const std::vector<uint8_t> &data) {
  std::vector<uint8_t> params;
  params.reserve(1 + data.size());
  params.push_back(static_cast<uint8_t>(address));
  params.insert(params.end(), data.begin(), data.end());
  return build_packet_(id, 0x03, params);
}

std::vector<uint8_t> build_reboot(uint8_t id) { return build_packet_(id, 0x08, {}); }

static bool read_find_header_(const ReadFn &read_fn, uint32_t timeout_ms) {
  uint8_t b = 0;
  if (!read_fn(&b, 1, timeout_ms)) {
    return false;
  }
  if (b != 0xFF) {
    return read_find_header_(read_fn, timeout_ms);
  }
  if (!read_fn(&b, 1, timeout_ms)) {
    return false;
  }
  return b == 0xFF;
}

bool read_status(const ReadFn &read_fn, StatusPacket &status, uint32_t timeout_ms) {
  if (!read_find_header_(read_fn, timeout_ms)) {
    return false;
  }
  uint8_t id = 0;
  uint8_t length = 0;
  uint8_t error = 0;
  if (!read_fn(&id, 1, timeout_ms) || !read_fn(&length, 1, timeout_ms) ||
      !read_fn(&error, 1, timeout_ms)) {
    return false;
  }
  if (length < 2) {
    return false;
  }
  std::vector<uint8_t> params(length - 2, 0);
  if (!params.empty()) {
    if (!read_fn(params.data(), params.size(), timeout_ms)) {
      return false;
    }
  }
  uint8_t rx_checksum = 0;
  if (!read_fn(&rx_checksum, 1, timeout_ms)) {
    return false;
  }
  std::vector<uint8_t> packet;
  packet.reserve(6 + params.size());
  packet.push_back(0xFF);
  packet.push_back(0xFF);
  packet.push_back(id);
  packet.push_back(length);
  packet.push_back(error);
  packet.insert(packet.end(), params.begin(), params.end());
  packet.push_back(rx_checksum);
  if (checksum_(packet) != rx_checksum) {
    return false;
  }
  status.id = id;
  status.error = error;
  status.parameters = params;
  return true;
}

}  // namespace protocol_v1
}  // namespace dynamixel
}  // namespace esphome
