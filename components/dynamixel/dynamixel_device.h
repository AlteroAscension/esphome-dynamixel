#pragma once

#include <vector>
#include <string>

#include "esphome/core/optional.h"

#include "dynamixel_types.h"

namespace esphome {
namespace dynamixel {

enum class RegisterAccess : uint8_t { READ = 1, WRITE = 2, READ_WRITE = 3 };

struct RegisterDef {
  std::string name;
  uint16_t address;
  uint16_t length;
  RegisterAccess access;
};

class DynamixelDevice {
 public:
  void set_name(const std::string &name) { this->name_ = name; }
  void set_device_id(uint8_t device_id) { this->device_id_ = device_id; }
  void set_protocol(uint8_t protocol);

  void add_register(const std::string &name, uint16_t address, uint16_t length,
                    const std::string &access);

  const std::string &get_name() const { return this->name_; }
  uint8_t get_device_id() const { return this->device_id_; }
  optional<ProtocolVersion> get_protocol() const { return this->protocol_; }
  const RegisterDef *find_register(const std::string &name) const;

 protected:
  std::string name_;
  uint8_t device_id_{0};
  optional<ProtocolVersion> protocol_;
  std::vector<RegisterDef> registers_;
};

}  // namespace dynamixel
}  // namespace esphome
