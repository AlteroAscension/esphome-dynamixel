#include "dynamixel_sensor.h"

#include "esphome/core/log.h"

namespace esphome {
namespace dynamixel {

static const char *const TAG = "dynamixel.sensor";

void DynamixelSensor::setup() {
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

static int32_t decode_value_(const std::vector<uint8_t> &data, bool is_signed) {
  uint32_t value = 0;
  for (size_t i = 0; i < data.size(); i++) {
    value |= static_cast<uint32_t>(data[i]) << (8 * i);
  }
  if (!is_signed) {
    return static_cast<int32_t>(value);
  }
  if (data.size() == 1) {
    return static_cast<int8_t>(value & 0xFF);
  }
  if (data.size() == 2) {
    return static_cast<int16_t>(value & 0xFFFF);
  }
  return static_cast<int32_t>(value);
}

void DynamixelSensor::update() {
  if (this->reg_ == nullptr || this->parent_ == nullptr) {
    return;
  }
  std::vector<uint8_t> out;
  if (!this->parent_->read_register(this->device_id_, this->reg_->name, out)) {
    ESP_LOGW(TAG, "Read failed for device %u, register '%s'", this->device_id_,
             this->reg_->name.c_str());
    return;
  }
  int32_t raw = decode_value_(out, this->signed_);
  float value = raw * this->multiply_ + this->offset_;
  this->publish_state(value);
}

}  // namespace dynamixel
}  // namespace esphome
