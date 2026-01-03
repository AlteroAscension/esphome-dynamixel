import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_DEVICE_ID, CONF_REGISTER, CONF_MULTIPLY, CONF_OFFSET

from . import dynamixel_ns, DynamixelComponent

DEPENDENCIES = ["dynamixel"]

DynamixelSensor = dynamixel_ns.class_("DynamixelSensor", sensor.Sensor, cg.PollingComponent)

CONFIG_SCHEMA = (
    sensor.sensor_schema(DynamixelSensor)
    .extend(
        {
            cv.Required("dynamixel_id"): cv.use_id(DynamixelComponent),
            cv.Required(CONF_DEVICE_ID): cv.uint8_t,
            cv.Required(CONF_REGISTER): cv.string,
            cv.Optional(CONF_MULTIPLY, default=1.0): cv.float_,
            cv.Optional(CONF_OFFSET, default=0.0): cv.float_,
            cv.Optional("signed", default=False): cv.boolean,
        }
    )
    .extend(cv.polling_component_schema("1s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await sensor.register_sensor(var, config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config["dynamixel_id"])
    cg.add(var.set_parent(parent))
    cg.add(var.set_device_id(config[CONF_DEVICE_ID]))
    cg.add(var.set_register_name(config[CONF_REGISTER]))
    cg.add(var.set_scale(config[CONF_MULTIPLY], config[CONF_OFFSET]))
    cg.add(var.set_signed(config["signed"]))
