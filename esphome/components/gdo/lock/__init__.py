import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import lock
from esphome.const import CONF_ID

from .. import GDO_CLIENT_SCHEMA, gdo_ns, register_gdo_child

DEPENDENCIES = ["gdo"]

GDOLock = gdo_ns.class_("GDOLock", lock.Lock, cg.Component)

CONFIG_SCHEMA = lock.LOCK_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(GDOLock),
    }
).extend(GDO_CLIENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await lock.register_lock(var, config)
    await cg.register_component(var, config)
    await register_gdo_child(var, config)
