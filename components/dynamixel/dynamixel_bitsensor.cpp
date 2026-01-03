#include "dynamixel_bitsensor.h"

#include "esphome/core/log.h"

namespace esphome {
namespace dynamixel {

static const char *const TAG = "dynamixel.binary_sensor";

void DynamixelBitSensor::setup() {
  if (this->parent_ == nullptr) {
    ESP_LOGE(TAG, "Parent component not set");
    return;
  }
  this->reg_ = this->parent_->find_register(this->device_id_, this->register_name_);
  if (this->reg_ == nullptr) {
    ESP_LOGE(TAG, "Register '%s' not found for device %u", this->register_name_.c_str(),
             this->device_id_);
  }
}

void DynamixelBitSensor::update() {
  if (this->parent_ == nullptr || this->reg_ == nullptr) {
    return;
  }
  std::vector<uint8_t> out;
  if (!this->parent_->read_register(this->device_id_, this->reg_->name, out)) {
    ESP_LOGW(TAG, "Read failed for device %u, register '%s'", this->device_id_,
             this->reg_->name.c_str());
    return;
  }
  uint8_t value = out.empty() ? 0 : out[0];
  bool state = (value & (1 << this->bit_)) != 0;
  this->publish_state(state);
}

}  // namespace dynamixel
}  // namespace esphome
