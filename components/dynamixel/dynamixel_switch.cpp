#include "dynamixel_switch.h"

#include "esphome/core/log.h"

namespace esphome {
namespace dynamixel {

static const char *const TAG = "dynamixel.switch";

bool DynamixelSwitch::resolve_register_() {
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

void DynamixelSwitch::write_state(bool state) {
  if (!this->resolve_register_()) {
    return;
  }
  std::vector<uint8_t> data(this->reg_->length, 0);
  if (!data.empty()) {
    data[0] = state ? 1 : 0;
  }
  if (!this->parent_->write_register(this->device_id_, this->reg_->name, data)) {
    ESP_LOGW(TAG, "Write failed for device %u, register '%s'", this->device_id_,
             this->reg_->name.c_str());
    return;
  }
  this->publish_state(state);
}

}  // namespace dynamixel
}  // namespace esphome
