import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    CONF_PRESSURE,
    CONF_TEMPERATURE,
    CONF_UNIT_OF_MEASUREMENT,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_HECTOPASCAL,
)

CONF_PRESSURE_UNITS = ["kPa", "hPa", "PSI", "Bar"]
CONF_PRESSURE_MIN = "pressure_range_min"
CONF_PRESSURE_MAX = "pressure_range_max"
DEPENDENCIES = ["i2c"]
CONF_I2C_ADDRESS = 0x6D

xdb3xx_ns = cg.esphome_ns.namespace("xdb3xx")


XDB3XXComponent = xdb3xx_ns.class_(
    "XDB3XXComponent", cg.PollingComponent, sensor.Sensor, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(XDB3XXComponent),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PRESSURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_HECTOPASCAL,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_PRESSURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PRESSURE_MIN, default=0): cv.int_,
            cv.Optional(CONF_PRESSURE_MAX, default=400): cv.int_,
            cv.Optional(CONF_UNIT_OF_MEASUREMENT): cv.one_of(CONF_PRESSURE_UNITS),
            cv.Optional(CONF_I2C_ADDRESS, default=0x6D): cv.i2c_address,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(CONF_I2C_ADDRESS))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if temperature_config := config.get(CONF_TEMPERATURE):
        sens = await sensor.new_sensor(temperature_config)
        cg.add(var.set_temperature_sensor(sens))
    if pressure_config := config.get(CONF_PRESSURE):
        sens = await sensor.new_sensor(pressure_config)
        cg.add(var.set_pressure_sensor(sens))
        if unit_config := config.get(CONF_UNIT_OF_MEASUREMENT):
            cg.add(var.set_unit_of_measurement(unit_config))
        cg.add(var.set_pressure_min(config[CONF_PRESSURE_MIN]))
        cg.add(var.set_pressure_max(config[CONF_PRESSURE_MAX]))