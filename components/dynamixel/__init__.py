import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome import pins
from esphome.const import CONF_ID, CONF_NAME

CONF_UART_ID = "uart_id"
CONF_PROTOCOL = "protocol"
CONF_DIRECTION_PIN = "direction_pin"
CONF_DEVICES = "devices"
CONF_DEVICE_ID = "device_id"
CONF_REGISTERS = "registers"
CONF_ADDRESS = "address"
CONF_LENGTH = "length"
CONF_ACCESS = "access"

dynamixel_ns = cg.esphome_ns.namespace("dynamixel")
DynamixelComponent = dynamixel_ns.class_("DynamixelComponent", cg.Component, uart.UARTDevice)

PROTOCOLS = cv.one_of(1, 2, int=True)
ACCESS = cv.one_of("r", "w", "rw", lower=True)

REGISTER_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_NAME): cv.string,
        cv.Required(CONF_ADDRESS): cv.uint16_t,
        cv.Required(CONF_LENGTH): cv.uint8_t,
        cv.Optional(CONF_ACCESS, default="rw"): ACCESS,
    }
)

DEVICE_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_NAME): cv.string,
        cv.Required(CONF_DEVICE_ID): cv.uint8_t,
        cv.Optional(CONF_PROTOCOL): PROTOCOLS,
        cv.Optional(CONF_REGISTERS, default=[]): cv.ensure_list(REGISTER_SCHEMA),
    }
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DynamixelComponent),
        cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
        cv.Optional(CONF_PROTOCOL, default=2): PROTOCOLS,
        cv.Optional(CONF_DIRECTION_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_DEVICES, default=[]): cv.ensure_list(DEVICE_SCHEMA),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_default_protocol(config[CONF_PROTOCOL]))

    if CONF_DIRECTION_PIN in config:
        pin = await cg.gpio_pin_expression(config[CONF_DIRECTION_PIN])
        cg.add(var.set_direction_pin(pin))

    for dev_cfg in config[CONF_DEVICES]:
        proto_set = CONF_PROTOCOL in dev_cfg
        proto_val = dev_cfg[CONF_PROTOCOL] if proto_set else 0
        cg.add(
            var.add_device(
                dev_cfg[CONF_DEVICE_ID],
                dev_cfg[CONF_NAME],
                proto_val,
                proto_set,
            )
        )
        for reg_cfg in dev_cfg[CONF_REGISTERS]:
            cg.add(
                var.add_device_register(
                    dev_cfg[CONF_DEVICE_ID],
                    reg_cfg[CONF_NAME],
                    reg_cfg[CONF_ADDRESS],
                    reg_cfg[CONF_LENGTH],
                    reg_cfg[CONF_ACCESS],
                )
            )
