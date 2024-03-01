import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID

from .. import GDO_CLIENT_SCHEMA, gdo_ns, register_gdo_child

DEPENDENCIES = ["gdo"]

GDOSwitch = gdo_ns.class_("GDOSwitch", switch.Switch, cg.Component)
SwitchType = gdo_ns.enum("SwitchType")

CONF_TYPE = "type"
TYPES = {
    "learn": SwitchType.GDO_LEARN,
}


CONFIG_SCHEMA = (
    switch.switch_schema(GDOSwitch)
    .extend(
        {
            cv.Required(CONF_TYPE): cv.enum(TYPES, lower=True),
        }
    )
    .extend(GDO_CLIENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await switch.register_switch(var, config)
    await cg.register_component(var, config)
    cg.add(var.set_switch_type(config[CONF_TYPE]))
    await register_gdo_child(var, config)
