import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import CONF_ID

from .. import GDO_CLIENT_SCHEMA, gdo_ns, register_gdo_child

DEPENDENCIES = ["gdo"]

GDONumber = gdo_ns.class_("GDONumber", number.Number, cg.Component)
NumberType = gdo_ns.enum("NumberType")

CONF_TYPE = "type"
TYPES = {
    "client_id": NumberType.GDO_CLIENT_ID,
    "rolling_code_counter": NumberType.GDO_ROLLING_CODE_COUNTER,
    "opening_duration": NumberType.GDO_OPENING_DURATION,
    "closing_duration": NumberType.GDO_CLOSING_DURATION,
}


CONFIG_SCHEMA = (
    number.number_schema(GDONumber)
    .extend(
        {
            cv.Required(CONF_TYPE): cv.enum(TYPES, lower=True),
        }
    )
    .extend(GDO_CLIENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await number.register_number(var, config, step=1, min_value=0, max_value=4294967295)
    await cg.register_component(var, config)
    cg.add(var.set_number_type(config[CONF_TYPE]))
    await register_gdo_child(var, config)
