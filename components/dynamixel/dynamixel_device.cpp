#include "dynamixel_device.h"

namespace esphome {
namespace dynamixel {

void DynamixelDevice::set_protocol(uint8_t protocol) {
  this->protocol_ = protocol == 1 ? ProtocolVersion::V1 : ProtocolVersion::V2;
}

void DynamixelDevice::add_register(const std::string &name, uint16_t address, uint16_t length,
                                   const std::string &access) {
  RegisterAccess acc = RegisterAccess::READ_WRITE;
  if (access == "r") {
    acc = RegisterAccess::READ;
  } else if (access == "w") {
    acc = RegisterAccess::WRITE;
  }
  this->registers_.push_back(RegisterDef{name, address, length, acc});
}

const RegisterDef *DynamixelDevice::find_register(const std::string &name) const {
  for (const auto &reg : this->registers_) {
    if (reg.name == name) {
      return &reg;
    }
  }
  return nullptr;
}

}  // namespace dynamixel
}  // namespace esphome
