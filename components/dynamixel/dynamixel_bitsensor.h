#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/core/component.h"

#include "dynamixel_component.h"

namespace esphome {
namespace dynamixel {

class DynamixelBitSensor : public binary_sensor::BinarySensor, public PollingComponent {
 public:
  void set_parent(DynamixelComponent *parent) { this->parent_ = parent; }
  void set_device_id(uint8_t device_id) { this->device_id_ = device_id; }
  void set_register_name(const std::string &name) { this->register_name_ = name; }
  void set_bit(uint8_t bit) { this->bit_ = bit; }

  void setup() override;
  void update() override;

 protected:
  DynamixelComponent *parent_{nullptr};
  uint8_t device_id_{0};
  std::string register_name_;
  const RegisterDef *reg_{nullptr};
  uint8_t bit_{0};
};

}  // namespace dynamixel
}  // namespace esphome
