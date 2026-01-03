#pragma once

#include <vector>
#include <string>

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/uart/uart.h"

#include "dynamixel_types.h"
#include "dynamixel_device.h"
#include "protocol_v1.h"
#include "protocol_v2.h"

namespace esphome {
namespace dynamixel {

class DynamixelComponent : public Component, public uart::UARTDevice {
 public:
  void set_direction_pin(GPIOPin *pin) { this->direction_pin_ = pin; }
  void set_default_protocol(uint8_t protocol);
  void add_device(uint8_t device_id, const std::string &name, uint8_t protocol,
                  bool protocol_set);
  void add_device_register(uint8_t device_id, const std::string &name, uint16_t address,
                           uint16_t length, const std::string &access);

  bool ping(uint8_t id, ProtocolVersion protocol);
  bool reboot(uint8_t id, ProtocolVersion protocol);
  bool read_register(uint8_t id, uint16_t address, uint16_t length, ProtocolVersion protocol,
                     std::vector<uint8_t> &out);
  bool write_register(uint8_t id, uint16_t address, const std::vector<uint8_t> &data,
                      ProtocolVersion protocol);

  // Helpers using device definitions
  bool read_register(uint8_t id, const std::string &reg_name, std::vector<uint8_t> &out);
  bool write_register(uint8_t id, const std::string &reg_name, const std::vector<uint8_t> &data);
  const RegisterDef *find_register(uint8_t id, const std::string &reg_name) const;
  void add_device_bit_sensor(uint8_t device_id, const std::string &name,
                             const std::string &reg_name, uint8_t bit);

  void setup() override;
  void dump_config() override;

 protected:
  bool write_packet_(const std::vector<uint8_t> &packet);
  bool read_status_v1_(protocol_v1::StatusPacket &status, uint32_t timeout_ms);
  bool read_status_v2_(protocol_v2::StatusPacket &status, uint32_t timeout_ms);
  bool read_bytes_(uint8_t *data, size_t len, uint32_t timeout_ms);

  ProtocolVersion default_protocol_{ProtocolVersion::V2};
  GPIOPin *direction_pin_{nullptr};
  std::vector<DynamixelDevice> devices_;
};

}  // namespace dynamixel
}  // namespace esphome
