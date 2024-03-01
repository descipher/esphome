import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID

from .. import GDO_CLIENT_SCHEMA, gdo_ns, register_gdo_child

DEPENDENCIES = ["gdo"]

GDOSensor = gdo_ns.class_("GDOSensor", sensor.Sensor, cg.Component)
GDOSensorType = gdo_ns.enum("GDOSensorType")

CONF_TYPE = "type"
TYPES = {
    "openings": GDOSensorType.GDO_OPENINGS,
    "paired_devices_total": GDOSensorType.GDO_PAIRED_DEVICES_TOTAL,
    "paired_devices_remotes": GDOSensorType.GDO_PAIRED_REMOTES,
    "paired_devices_keypads": GDOSensorType.GDO_PAIRED_KEYPADS,
    "paired_devices_wall_controls": GDOSensorType.GDO_PAIRED_WALL_CONTROLS,
    "paired_devices_accessories": GDOSensorType.GDO_PAIRED_ACCESSORIES,
}


CONFIG_SCHEMA = (
    sensor.sensor_schema(GDOSensor)
    .extend(
        {
            cv.Required(CONF_TYPE): cv.enum(TYPES, lower=True),
        }
    )
    .extend(GDO_CLIENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await sensor.register_sensor(var, config)
    await cg.register_component(var, config)
    cg.add(var.set_gdo_sensor_type(config[CONF_TYPE]))
    await register_gdo_child(var, config)
