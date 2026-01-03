#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

#include "dynamixel_component.h"

namespace esphome {
namespace dynamixel {

class DynamixelSensor : public sensor::Sensor, public PollingComponent {
 public:
  void set_parent(DynamixelComponent *parent) { this->parent_ = parent; }
  void set_device_id(uint8_t device_id) { this->device_id_ = device_id; }
  void set_register_name(const std::string &name) { this->register_name_ = name; }
  void set_scale(float multiply, float offset) {
    this->multiply_ = multiply;
    this->offset_ = offset;
  }
  void set_signed(bool is_signed) { this->signed_ = is_signed; }

  void setup() override;
  void update() override;

 protected:
  DynamixelComponent *parent_{nullptr};
  uint8_t device_id_{0};
  std::string register_name_;
  const RegisterDef *reg_{nullptr};
  float multiply_{1.0f};
  float offset_{0.0f};
  bool signed_{false};
};

}  // namespace dynamixel
}  // namespace esphome
