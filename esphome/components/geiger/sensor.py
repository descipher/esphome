import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart, time
from esphome.const import (
    CONF_VOLTAGE,
    CONF_ID,
    CONF_TIME_ID,
    CONF_MODEL,
    STATE_CLASS_MEASUREMENT,
    DEVICE_CLASS_VOLTAGE,
    UNIT_VOLT,
)

CONF_GEIGER_CPM = "geiger_cpm"
CONF_GEIGER_CPS = "geiger_cps"
CONF_GEIGER_DOSE = "geiger_dose"
CONF_GEIGER_TUBE = "geiger_tube"
CONF_GEIGER_SOUND = "geiger_sound"
UNIT_COUNT_PER_SECOND = "cps"
UNIT_COUNT_PER_MINUTE = "cpm"
UNIT_MICROSIEVERTS_PER_HOUR = "µSv/h"
DEVICE_CLASS_RADIATION = "radiation"

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@descipher"]

geiger_ns = cg.esphome_ns.namespace("geiger")
GEIGERModel = geiger_ns.enum("GEIGERModel")
GEIGERTube = geiger_ns.enum("GEIGERTube")

GEIGER_MODELS = {
    "GRGC1": GEIGERModel.GEIGER_MODEL_GRGC1,
    "GRGC2": GEIGERModel.GEIGER_MODEL_GRGC2,
}
GEIGER_TUBES = {
    "Unknown": GEIGERTube.GEIGER_TUBE_UNKNOWN,
    "SBM20": GEIGERTube.GEIGER_TUBE_SBM20,
    "SI29BG": GEIGERTube.GEIGER_TUBE_SI29BG,
    "SBM19": GEIGERTube.GEIGER_TUBE_SBM19,
    "LND712": GEIGERTube.GEIGER_TUBE_LND712,
    "SBM20M": GEIGERTube.GEIGER_TUBE_SBM20M,
    "SI22G": GEIGERTube.GEIGER_TUBE_SI22G,
    "STS5": GEIGERTube.GEIGER_TUBE_STS5,
    "SI3BG": GEIGERTube.GEIGER_TUBE_SI3BG,
    "SBT9": GEIGERTube.GEIGER_TUBE_SBT9,
    "SI1G": GEIGERTube.GEIGER_TUBE_SI1G,
    "J305": GEIGERTube.GEIGER_TUBE_J305,
    "M4011": GEIGERTube.GEIGER_TUBE_M4011,
}

GEIGERComponent = geiger_ns.class_("GEIGER", cg.PollingComponent, uart.UARTDevice)


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(GEIGERComponent),
            cv.Optional(CONF_GEIGER_CPM): sensor.sensor_schema(
                unit_of_measurement=UNIT_COUNT_PER_MINUTE,
                # device_class=DEVICE_CLASS_RADIATION,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_GEIGER_CPS): sensor.sensor_schema(
                unit_of_measurement=UNIT_COUNT_PER_SECOND,
                # device_class=DEVICE_CLASS_RADIATION,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_GEIGER_DOSE): sensor.sensor_schema(
                unit_of_measurement=UNIT_MICROSIEVERTS_PER_HOUR,
                accuracy_decimals=6,
                # device_class=DEVICE_CLASS_RADIATION,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                device_class=DEVICE_CLASS_VOLTAGE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_MODEL, default="GRGC1"): cv.enum(
                GEIGER_MODELS, upper=True, space="_"
            ),
            cv.Optional(CONF_GEIGER_SOUND): cv.boolean,
            cv.Optional(CONF_GEIGER_TUBE, default="SBM20"): cv.enum(
                GEIGER_TUBES,
            ),
            cv.Optional(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
        }
    )
    .extend(cv.polling_component_schema("10s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_GEIGER_CPM in config:
        sens = await sensor.new_sensor(config[CONF_GEIGER_CPM])
        cg.add(var.set_geiger_cpm(sens))
    if CONF_GEIGER_CPS in config:
        sens = await sensor.new_sensor(config[CONF_GEIGER_CPS])
        cg.add(var.set_geiger_cps(sens))
    if CONF_GEIGER_DOSE in config:
        sens = await sensor.new_sensor(config[CONF_GEIGER_DOSE])
        cg.add(var.set_geiger_dose(sens))
    if CONF_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_VOLTAGE])
        cg.add(var.set_geiger_voltage(sens))
    if CONF_TIME_ID in config:
        time_ = await cg.get_variable(config[CONF_TIME_ID])
        cg.add(var.set_time_id(time_))
    cg.add(var.set_geiger_model(config[CONF_MODEL]))
    cg.add(var.set_geiger_sound(config[CONF_GEIGER_SOUND]))
    cg.add(var.set_geiger_tube(config[CONF_GEIGER_TUBE]))
