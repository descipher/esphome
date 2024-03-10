import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID

from .. import GDO_CLIENT_SCHEMA, gdo_ns, register_gdo_child

DEPENDENCIES = ["gdo"]

GDOBinarySensor = gdo_ns.class_(
    "GDOBinarySensor", binary_sensor.BinarySensor, cg.Component
)
SensorType = gdo_ns.enum("SensorType")

CONF_TYPE = "type"
TYPES = {
    "motion": SensorType.GDO_SENSOR_MOTION,
    "obstruction": SensorType.GDO_SENSOR_OBSTRUCTION,
    "motor": SensorType.GDO_SENSOR_MOTOR,
    "button": SensorType.GDO_SENSOR_BUTTON,
}


CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(GDOBinarySensor)
    .extend(
        {
            cv.Required(CONF_TYPE): cv.enum(TYPES, lower=True),
        }
    )
    .extend(GDO_CLIENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)
    await cg.register_component(var, config)
    cg.add(var.set_binary_sensor_type(config[CONF_TYPE]))
    await register_gdo_child(var, config)
