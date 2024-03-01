import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_OUTPUT_ID  # New in 2023.5

from .. import GDO_CLIENT_SCHEMA, gdo_ns, register_gdo_child

DEPENDENCIES = ["gdo"]

GDOLightOutput = gdo_ns.class_(
    "GDOLightOutput", light.LightOutput, cg.Component
)


CONFIG_SCHEMA = light.LIGHT_SCHEMA.extend(
    {cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(GDOLightOutput)}
).extend(GDO_CLIENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await cg.register_component(var, config)
    await light.register_light(var, config)
    await register_gdo_child(var, config)
