#include "dynamixel_number.h"

#include <cmath>

#include "esphome/core/log.h"

namespace esphome {
namespace dynamixel {

static const char *const TAG = "dynamixel.number";

bool DynamixelNumber::resolve_register_() {
  if (this->parent_ == nullptr) {
    ESP_LOGE(TAG, "Parent component not set");
    return false;
  }
  if (this->reg_ == nullptr) {
    this->reg_ = this->parent_->find_register(this->device_id_, this->register_name_);
    if (this->reg_ == nullptr) {
      ESP_LOGE(TAG, "Register '%s' not found for device %u", this->register_name_.c_str(),
               this->device_id_);
      return false;
    }
  }
  return true;
}

void DynamixelNumber::control(float value) {
  if (!this->resolve_register_()) {
    return;
  }
  int32_t v = static_cast<int32_t>(lroundf(value));
  std::vector<uint8_t> data;
  if (this->reg_->length == 1) {
    data = {static_cast<uint8_t>(v & 0xFF)};
  } else if (this->reg_->length == 2) {
    data = {static_cast<uint8_t>(v & 0xFF), static_cast<uint8_t>((v >> 8) & 0xFF)};
  } else if (this->reg_->length == 4) {
    data = {static_cast<uint8_t>(v & 0xFF), static_cast<uint8_t>((v >> 8) & 0xFF),
            static_cast<uint8_t>((v >> 16) & 0xFF), static_cast<uint8_t>((v >> 24) & 0xFF)};
  } else {
    ESP_LOGW(TAG, "Unsupported register length %u for '%s'", this->reg_->length,
             this->reg_->name.c_str());
    return;
  }
  if (!this->parent_->write_register(this->device_id_, this->reg_->name, data)) {
    ESP_LOGW(TAG, "Write failed for device %u, register '%s'", this->device_id_,
             this->reg_->name.c_str());
    return;
  }
  this->publish_state(value);
}

}  // namespace dynamixel
}  // namespace esphome
