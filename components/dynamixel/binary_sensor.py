import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID

from . import dynamixel_ns, DynamixelComponent

CONF_DXL_DEVICE_ID = "dxl_device_id"

DEPENDENCIES = ["dynamixel"]

DynamixelBitSensor = dynamixel_ns.class_(
    "DynamixelBitSensor", binary_sensor.BinarySensor, cg.PollingComponent
)

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(DynamixelBitSensor)
    .extend(
        {
            cv.Required("dynamixel_id"): cv.use_id(DynamixelComponent),
            cv.Required(CONF_DXL_DEVICE_ID): cv.uint8_t,
            cv.Required("register"): cv.string,
            cv.Required("bit"): cv.int_range(min=0, max=7),
        }
    )
    .extend(cv.polling_component_schema("1s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config["dynamixel_id"])
    cg.add(var.set_parent(parent))
    cg.add(var.set_device_id(config[CONF_DXL_DEVICE_ID]))
    cg.add(var.set_register_name(config["register"]))
    cg.add(var.set_bit(config["bit"]))
