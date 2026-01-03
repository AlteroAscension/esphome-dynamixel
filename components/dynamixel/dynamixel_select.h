#pragma once

#include <vector>
#include <string>

#include "esphome/components/select/select.h"

#include "dynamixel_component.h"

namespace esphome {
namespace dynamixel {

class DynamixelSelect : public select::Select {
 public:
  void set_parent(DynamixelComponent *parent) { this->parent_ = parent; }
  void set_device_id(uint8_t device_id) { this->device_id_ = device_id; }
  void set_register_name(const std::string &name) { this->register_name_ = name; }
  void add_option_value(const std::string &name, int32_t value) {
    this->option_values_.push_back({name, value});
  }

 protected:
  void control(const std::string &value) override;
  bool resolve_register_();

  DynamixelComponent *parent_{nullptr};
  uint8_t device_id_{0};
  std::string register_name_;
  const RegisterDef *reg_{nullptr};
  std::vector<std::pair<std::string, int32_t>> option_values_;
};

}  // namespace dynamixel
}  // namespace esphome
