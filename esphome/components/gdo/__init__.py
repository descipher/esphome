import esphome.codegen as cg
import esphome.config_validation as cv
import voluptuous as vol
from esphome import automation, pins

# from esphome.components import uart
from esphome.const import CONF_ID, CONF_TRIGGER_ID

DEPENDENCIES = ["preferences"]
MULTI_CONF = True


gdo_ns = cg.esphome_ns.namespace("gdo")
GDOComponent = gdo_ns.class_("GDOComponent", cg.Component)


SyncFailed = gdo_ns.class_("SyncFailed", automation.Trigger.template())

CONF_OUTPUT_GDO = "output_gdo_pin"
DEFAULT_OUTPUT_GDO = "GPIO5"  # UART1 TX drives on the Red terminal of the GDO (Red terminal on GRGDO1 hardware)
CONF_INPUT_GDO = "input_gdo_pin"
DEFAULT_INPUT_GDO = "GPIO4"  # UART1 RX listens on the Red terminal of the GDO also (Red terminal on GRGDO1 hardware)
CONF_INPUT_OBST = "input_obst_pin"
DEFAULT_INPUT_OBST = "GPIO12"  # Signal based optical obstruction sensor input
CONF_INPUT_RF = "input_rf_pin"
DEFAULT_INPUT_RF = "GPIO14"
CONF_OUTPUT_RF = "output_rf_pin"
DEFAULT_OUTPUT_RF = "GPIO16"
CONF_GDO_ID = "gdo_id"
CONF_ON_SYNC_FAILED = "on_sync_failed"
CONF_PROTOCOL = "protocol"
PROTOCOL_SECPLUSV1 = "secplusv1"
PROTOCOL_SECPLUSV2 = "secplusv2"
PROTOCOL_DRYCONTACT = "drycontact"
SUPPORTED_PROTOCOLS = [PROTOCOL_SECPLUSV1, PROTOCOL_SECPLUSV2, PROTOCOL_DRYCONTACT]

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(GDOComponent),
            cv.Optional(
                CONF_OUTPUT_GDO, default=DEFAULT_OUTPUT_GDO
            ): pins.gpio_output_pin_schema,
            cv.Optional(
                CONF_INPUT_GDO, default=DEFAULT_INPUT_GDO
            ): pins.gpio_input_pin_schema,
            cv.Optional(CONF_INPUT_OBST, default=DEFAULT_INPUT_OBST): cv.Any(
                cv.none, pins.gpio_input_pin_schema
            ),
            cv.Optional(
                CONF_INPUT_RF, default=DEFAULT_INPUT_RF
            ): pins.gpio_input_pin_schema,
            cv.Optional(
                CONF_OUTPUT_RF, default=DEFAULT_OUTPUT_RF
            ): pins.gpio_input_pin_schema,
            cv.Optional(CONF_ON_SYNC_FAILED): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(SyncFailed),
                }
            ),
            cv.Optional(CONF_PROTOCOL, default=PROTOCOL_SECPLUSV2): vol.In(
                SUPPORTED_PROTOCOLS
            ),
        }
    )
    # .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)

GDO_CLIENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_GDO_ID): cv.use_id(GDOComponent),
    }
)


async def register_gdo_child(var, config):
    parent = await cg.get_variable(config[CONF_GDO_ID])
    cg.add(var.set_parent(parent))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    pin = await cg.gpio_pin_expression(config[CONF_OUTPUT_GDO])
    cg.add(var.set_output_gdo_pin(pin))
    pin = await cg.gpio_pin_expression(config[CONF_INPUT_GDO])
    cg.add(var.set_input_gdo_pin(pin))
    if obst_input_config := config.get(CONF_INPUT_OBST):
        pin = await cg.gpio_pin_expression(obst_input_config)
        cg.add(var.set_rf_rx_pin(pin))
    if rf_input_config := config.get(CONF_INPUT_RF):
        pin = await cg.gpio_pin_expression(rf_input_config)
        cg.add(var.set_rf_rx_pin(pin))
    if rf_output_config := config.get(CONF_OUTPUT_RF):
        pin = await cg.gpio_pin_expression(rf_output_config)
        cg.add(var.set_rf_tx_pin(pin))
    for conf in config.get(CONF_ON_SYNC_FAILED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    cg.add_library(
        name="secplus",
        repository="https://github.com/descipher/secplus",
        version=None,
    )
    cg.add_library(
        name="espsoftwareserial",
        repository="https://github.com/descipher/espsoftwareserial",
        version=None,
    )
    # cg.add_library(
    #     name="arduino-esp32",
    #     repository="https://github.com/espressif/arduino-esp32",
    #     version="3.0.0",
    # )

    if config[CONF_PROTOCOL] == PROTOCOL_SECPLUSV1:
        cg.add_define("PROTOCOL_SECPLUSV1")
    elif config[CONF_PROTOCOL] == PROTOCOL_SECPLUSV2:
        cg.add_define("PROTOCOL_SECPLUSV2")
    elif config[CONF_PROTOCOL] == PROTOCOL_DRYCONTACT:
        cg.add_define("PROTOCOL_DRYCONTACT")
    cg.add(var.init_protocol())
