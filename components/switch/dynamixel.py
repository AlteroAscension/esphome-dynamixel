import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID, CONF_DEVICE_ID, CONF_REGISTER

from esphome.components.dynamixel import dynamixel_ns, DynamixelComponent

DEPENDENCIES = ["dynamixel"]

DynamixelSwitch = dynamixel_ns.class_("DynamixelSwitch", switch.Switch)

CONFIG_SCHEMA = switch.switch_schema(DynamixelSwitch).extend(
    {
        cv.Required("dynamixel_id"): cv.use_id(DynamixelComponent),
        cv.Required(CONF_DEVICE_ID): cv.uint8_t,
        cv.Required(CONF_REGISTER): cv.string,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await switch.register_switch(var, config)

    parent = await cg.get_variable(config["dynamixel_id"])
    cg.add(var.set_parent(parent))
    cg.add(var.set_device_id(config[CONF_DEVICE_ID]))
    cg.add(var.set_register_name(config[CONF_REGISTER]))
