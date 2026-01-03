#include "dynamixel_component.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dynamixel {

static const char *const TAG = "dynamixel";

void DynamixelComponent::set_default_protocol(uint8_t protocol) {
  this->default_protocol_ = protocol == 1 ? ProtocolVersion::V1 : ProtocolVersion::V2;
}

void DynamixelComponent::setup() {
  if (this->direction_pin_ != nullptr) {
    this->direction_pin_->setup();
    this->direction_pin_->digital_write(false);
  }
}

void DynamixelComponent::add_device(uint8_t device_id, const std::string &name, uint8_t protocol,
                                    bool protocol_set) {
  DynamixelDevice dev;
  dev.set_device_id(device_id);
  dev.set_name(name);
  if (protocol_set) {
    dev.set_protocol(protocol);
  }
  this->devices_.push_back(dev);
}

void DynamixelComponent::add_device_register(uint8_t device_id, const std::string &name,
                                             uint16_t address, uint16_t length,
                                             const std::string &access) {
  for (auto &dev : this->devices_) {
    if (dev.get_device_id() == device_id) {
      dev.add_register(name, address, length, access);
      return;
    }
  }
}

void DynamixelComponent::add_device_bit_sensor(uint8_t device_id, const std::string &name,
                                               const std::string &reg_name, uint8_t bit) {
  (void) device_id;
  (void) name;
  (void) reg_name;
  (void) bit;
  // No-op placeholder to keep codegen compatibility if needed later.
}

bool DynamixelComponent::ping(uint8_t id, ProtocolVersion protocol) {
  std::vector<uint8_t> packet;
  if (protocol == ProtocolVersion::V1) {
    packet = protocol_v1::build_ping(id);
  } else {
    packet = protocol_v2::build_ping(id);
  }
  if (!this->write_packet_(packet)) {
    return false;
  }
  if (protocol == ProtocolVersion::V1) {
    protocol_v1::StatusPacket status;
    return this->read_status_v1_(status, 50);
  }
  protocol_v2::StatusPacket status;
  return this->read_status_v2_(status, 50);
}

bool DynamixelComponent::reboot(uint8_t id, ProtocolVersion protocol) {
  std::vector<uint8_t> packet;
  if (protocol == ProtocolVersion::V1) {
    packet = protocol_v1::build_reboot(id);
  } else {
    packet = protocol_v2::build_reboot(id);
  }
  if (!this->write_packet_(packet)) {
    return false;
  }
  if (protocol == ProtocolVersion::V1) {
    protocol_v1::StatusPacket status;
    return this->read_status_v1_(status, 50);
  }
  protocol_v2::StatusPacket status;
  return this->read_status_v2_(status, 50);
}

bool DynamixelComponent::read_register(uint8_t id, uint16_t address, uint16_t length,
                                       ProtocolVersion protocol, std::vector<uint8_t> &out) {
  std::vector<uint8_t> packet =
      protocol == ProtocolVersion::V1 ? protocol_v1::build_read(id, address, length)
                                      : protocol_v2::build_read(id, address, length);
  if (!this->write_packet_(packet)) {
    return false;
  }
  if (protocol == ProtocolVersion::V1) {
    protocol_v1::StatusPacket status;
    if (!this->read_status_v1_(status, 80)) {
      return false;
    }
    out = status.parameters;
    return true;
  }
  protocol_v2::StatusPacket status;
  if (!this->read_status_v2_(status, 80)) {
    return false;
  }
  out = status.parameters;
  return true;
}

bool DynamixelComponent::write_register(uint8_t id, uint16_t address,
                                        const std::vector<uint8_t> &data,
                                        ProtocolVersion protocol) {
  std::vector<uint8_t> packet =
      protocol == ProtocolVersion::V1 ? protocol_v1::build_write(id, address, data)
                                      : protocol_v2::build_write(id, address, data);
  if (!this->write_packet_(packet)) {
    return false;
  }
  if (protocol == ProtocolVersion::V1) {
    protocol_v1::StatusPacket status;
    return this->read_status_v1_(status, 80);
  }
  protocol_v2::StatusPacket status;
  return this->read_status_v2_(status, 80);
}

bool DynamixelComponent::read_register(uint8_t id, const std::string &reg_name,
                                       std::vector<uint8_t> &out) {
  for (auto &dev : this->devices_) {
    if (dev.get_device_id() == id) {
      const RegisterDef *reg = dev.find_register(reg_name);
      if (reg == nullptr) {
        return false;
      }
      ProtocolVersion proto = dev.get_protocol().value_or(this->default_protocol_);
      return this->read_register(id, reg->address, reg->length, proto, out);
    }
  }
  return false;
}

bool DynamixelComponent::write_register(uint8_t id, const std::string &reg_name,
                                        const std::vector<uint8_t> &data) {
  for (auto &dev : this->devices_) {
    if (dev.get_device_id() == id) {
      const RegisterDef *reg = dev.find_register(reg_name);
      if (reg == nullptr) {
        return false;
      }
      ProtocolVersion proto = dev.get_protocol().value_or(this->default_protocol_);
      return this->write_register(id, reg->address, data, proto);
    }
  }
  return false;
}

const RegisterDef *DynamixelComponent::find_register(uint8_t id,
                                                     const std::string &reg_name) const {
  for (const auto &dev : this->devices_) {
    if (dev.get_device_id() == id) {
      return dev.find_register(reg_name);
    }
  }
  return nullptr;
}

void DynamixelComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Dynamixel:");
  ESP_LOGCONFIG(TAG, "  Default protocol: %u", this->default_protocol_ == ProtocolVersion::V1 ? 1 : 2);
  if (this->direction_pin_ != nullptr) {
    ESP_LOGCONFIG(TAG, "  Direction pin: configured");
  }
  ESP_LOGCONFIG(TAG, "  Devices: %u", static_cast<unsigned>(this->devices_.size()));
}

bool DynamixelComponent::write_packet_(const std::vector<uint8_t> &packet) {
  if (this->direction_pin_ != nullptr) {
    this->direction_pin_->digital_write(true);
    delayMicroseconds(10);
  }
  this->write_array(packet);
  this->flush();
  if (this->direction_pin_ != nullptr) {
    delayMicroseconds(10);
    this->direction_pin_->digital_write(false);
  }
  return true;
}

bool DynamixelComponent::read_bytes_(uint8_t *data, size_t len, uint32_t timeout_ms) {
  const uint32_t start = millis();
  size_t offset = 0;
  while (offset < len) {
    if (millis() - start > timeout_ms) {
      return false;
    }
    if (!this->available()) {
      delay(1);
      continue;
    }
    int c = this->read();
    if (c < 0) {
      continue;
    }
    data[offset++] = static_cast<uint8_t>(c);
  }
  return true;
}

bool DynamixelComponent::read_status_v1_(protocol_v1::StatusPacket &status, uint32_t timeout_ms) {
  return protocol_v1::read_status([this](uint8_t *data, size_t len, uint32_t to_ms) {
    return this->read_bytes_(data, len, to_ms);
  }, status, timeout_ms);
}

bool DynamixelComponent::read_status_v2_(protocol_v2::StatusPacket &status, uint32_t timeout_ms) {
  return protocol_v2::read_status([this](uint8_t *data, size_t len, uint32_t to_ms) {
    return this->read_bytes_(data, len, to_ms);
  }, status, timeout_ms);
}

}  // namespace dynamixel
}  // namespace esphome
