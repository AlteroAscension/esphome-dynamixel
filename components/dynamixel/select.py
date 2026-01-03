import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import CONF_ID

from . import dynamixel_ns, DynamixelComponent

CONF_DXL_DEVICE_ID = "dxl_device_id"

DEPENDENCIES = ["dynamixel"]

DynamixelSelect = dynamixel_ns.class_("DynamixelSelect", select.Select)

OPTION_SCHEMA = cv.Schema(
    {
        cv.Required("name"): cv.string,
        cv.Required("value"): cv.int_,
    }
)

CONFIG_SCHEMA = select.select_schema(DynamixelSelect).extend(
    {
        cv.Required("dynamixel_id"): cv.use_id(DynamixelComponent),
        cv.Required(CONF_DXL_DEVICE_ID): cv.uint8_t,
        cv.Required("register"): cv.string,
        cv.Required("options"): cv.ensure_list(OPTION_SCHEMA),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await select.register_select(var, config, options=[o["name"] for o in config["options"]])

    parent = await cg.get_variable(config["dynamixel_id"])
    cg.add(var.set_parent(parent))
    cg.add(var.set_device_id(config[CONF_DXL_DEVICE_ID]))
    cg.add(var.set_register_name(config["register"]))
    for opt in config["options"]:
        cg.add(var.add_option_value(opt["name"], opt["value"]))
