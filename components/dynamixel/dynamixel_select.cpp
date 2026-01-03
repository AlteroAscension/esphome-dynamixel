#include "dynamixel_select.h"

#include "esphome/core/log.h"

namespace esphome {
namespace dynamixel {

static const char *const TAG = "dynamixel.select";

bool DynamixelSelect::resolve_register_() {
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

void DynamixelSelect::control(const std::string &value) {
  if (!this->resolve_register_()) {
    return;
  }
  int32_t reg_value = 0;
  bool found = false;
  for (const auto &opt : this->option_values_) {
    if (opt.first == value) {
      reg_value = opt.second;
      found = true;
      break;
    }
  }
  if (!found) {
    ESP_LOGW(TAG, "Option '%s' not found", value.c_str());
    return;
  }
  std::vector<uint8_t> data;
  if (this->reg_->length == 1) {
    data = {static_cast<uint8_t>(reg_value & 0xFF)};
  } else if (this->reg_->length == 2) {
    data = {static_cast<uint8_t>(reg_value & 0xFF), static_cast<uint8_t>((reg_value >> 8) & 0xFF)};
  } else if (this->reg_->length == 4) {
    data = {static_cast<uint8_t>(reg_value & 0xFF), static_cast<uint8_t>((reg_value >> 8) & 0xFF),
            static_cast<uint8_t>((reg_value >> 16) & 0xFF),
            static_cast<uint8_t>((reg_value >> 24) & 0xFF)};
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
