import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import CONF_ID

from . import dynamixel_ns, DynamixelComponent

CONF_DXL_DEVICE_ID = "dxl_device_id"

DEPENDENCIES = ["dynamixel"]

DynamixelNumber = dynamixel_ns.class_("DynamixelNumber", number.Number)

CONFIG_SCHEMA = number.number_schema(DynamixelNumber).extend(
    {
        cv.Required("dynamixel_id"): cv.use_id(DynamixelComponent),
        cv.Required(CONF_DXL_DEVICE_ID): cv.uint8_t,
        cv.Required("register"): cv.string,
        cv.Optional("min_value"): cv.float_,
        cv.Optional("max_value"): cv.float_,
        cv.Optional("step"): cv.float_,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    min_value = config.get("min_value", 0.0)
    max_value = config.get("max_value", 0.0)
    step = config.get("step", 1.0)
    await number.register_number(var, config, min_value=min_value, max_value=max_value, step=step)

    parent = await cg.get_variable(config["dynamixel_id"])
    cg.add(var.set_parent(parent))
    cg.add(var.set_device_id(config[CONF_DXL_DEVICE_ID]))
    cg.add(var.set_register_name(config["register"]))
