#pragma once

#include "esphome/components/number/number.h"

#include "dynamixel_component.h"

namespace esphome {
namespace dynamixel {

class DynamixelNumber : public number::Number {
 public:
  void set_parent(DynamixelComponent *parent) { this->parent_ = parent; }
  void set_device_id(uint8_t device_id) { this->device_id_ = device_id; }
  void set_register_name(const std::string &name) { this->register_name_ = name; }

 protected:
  void control(float value) override;
  bool resolve_register_();

  DynamixelComponent *parent_{nullptr};
  uint8_t device_id_{0};
  std::string register_name_;
  const RegisterDef *reg_{nullptr};
};

}  // namespace dynamixel
}  // namespace esphome
