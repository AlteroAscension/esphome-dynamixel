#include "protocol_v2.h"

namespace esphome {
namespace dynamixel {
namespace protocol_v2 {

static uint16_t crc_update_(uint16_t crc_accum, const uint8_t *data, size_t size) {
  for (size_t j = 0; j < size; j++) {
    crc_accum ^= static_cast<uint16_t>(data[j]) << 8;
    for (uint8_t i = 0; i < 8; i++) {
      if (crc_accum & 0x8000) {
        crc_accum = (crc_accum << 1) ^ 0x8005;
      } else {
        crc_accum = (crc_accum << 1);
      }
    }
  }
  return crc_accum;
}

static std::vector<uint8_t> build_packet_(uint8_t id, uint8_t instruction,
                                          const std::vector<uint8_t> &params) {
  std::vector<uint8_t> packet;
  packet.reserve(10 + params.size());
  packet.push_back(0xFF);
  packet.push_back(0xFF);
  packet.push_back(0xFD);
  packet.push_back(0x00);
  packet.push_back(id);
  const uint16_t length = static_cast<uint16_t>(params.size() + 3);
  packet.push_back(length & 0xFF);
  packet.push_back((length >> 8) & 0xFF);
  packet.push_back(instruction);
  packet.insert(packet.end(), params.begin(), params.end());
  uint16_t crc = crc_update_(0, packet.data(), packet.size());
  packet.push_back(crc & 0xFF);
  packet.push_back((crc >> 8) & 0xFF);
  return packet;
}

std::vector<uint8_t> build_ping(uint8_t id) { return build_packet_(id, 0x01, {}); }

std::vector<uint8_t> build_read(uint8_t id, uint16_t address, uint16_t length) {
  std::vector<uint8_t> params{static_cast<uint8_t>(address & 0xFF),
                              static_cast<uint8_t>((address >> 8) & 0xFF),
                              static_cast<uint8_t>(length & 0xFF),
                              static_cast<uint8_t>((length >> 8) & 0xFF)};
  return build_packet_(id, 0x02, params);
}

std::vector<uint8_t> build_write(uint8_t id, uint16_t address, const std::vector<uint8_t> &data) {
  std::vector<uint8_t> params;
  params.reserve(2 + data.size());
  params.push_back(static_cast<uint8_t>(address & 0xFF));
  params.push_back(static_cast<uint8_t>((address >> 8) & 0xFF));
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
  if (!read_fn(&b, 1, timeout_ms) || b != 0xFF) {
    return false;
  }
  if (!read_fn(&b, 1, timeout_ms) || b != 0xFD) {
    return false;
  }
  if (!read_fn(&b, 1, timeout_ms) || b != 0x00) {
    return false;
  }
  return true;
}

bool read_status(const ReadFn &read_fn, StatusPacket &status, uint32_t timeout_ms) {
  if (!read_find_header_(read_fn, timeout_ms)) {
    return false;
  }
  uint8_t id = 0;
  uint8_t len_l = 0;
  uint8_t len_h = 0;
  uint8_t inst = 0;
  if (!read_fn(&id, 1, timeout_ms) || !read_fn(&len_l, 1, timeout_ms) ||
      !read_fn(&len_h, 1, timeout_ms) || !read_fn(&inst, 1, timeout_ms)) {
    return false;
  }
  const uint16_t length = static_cast<uint16_t>(len_l | (len_h << 8));
  if (length < 3) {
    return false;
  }
  std::vector<uint8_t> params(length - 3, 0);
  if (!params.empty()) {
    if (!read_fn(params.data(), params.size(), timeout_ms)) {
      return false;
    }
  }
  uint8_t crc_l = 0;
  uint8_t crc_h = 0;
  if (!read_fn(&crc_l, 1, timeout_ms) || !read_fn(&crc_h, 1, timeout_ms)) {
    return false;
  }

  std::vector<uint8_t> packet;
  packet.reserve(10 + params.size());
  packet.push_back(0xFF);
  packet.push_back(0xFF);
  packet.push_back(0xFD);
  packet.push_back(0x00);
  packet.push_back(id);
  packet.push_back(len_l);
  packet.push_back(len_h);
  packet.push_back(inst);
  packet.insert(packet.end(), params.begin(), params.end());
  uint16_t crc = crc_update_(0, packet.data(), packet.size());
  uint16_t rx_crc = static_cast<uint16_t>(crc_l | (crc_h << 8));
  if (crc != rx_crc) {
    return false;
  }
  status.id = id;
  status.error = params.size() >= 1 ? params[0] : 0;
  if (params.size() >= 1) {
    status.parameters.assign(params.begin() + 1, params.end());
  }
  return true;
}

}  // namespace protocol_v2
}  // namespace dynamixel
}  // namespace esphome
