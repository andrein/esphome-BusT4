import esphome.codegen as cg
from esphome.components import cover
import esphome.config_validation as cv
from esphome.const import CONF_ID

from .. import CONF_BUS_T4_ID, BusT4Component, bus_t4_ns

DEPENDENCIES = ['bus_t4']

BusT4Cover = bus_t4_ns.class_('BusT4Cover', cover.Cover, cg.Component)

CONFIG_SCHEMA = cv.All(
    cover.COVER_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(BusT4Cover),
            cv.GenerateID(CONF_BUS_T4_ID): cv.use_id(BusT4Component),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cover.register_cover(var, config)

    parent = await cg.get_variable(config[CONF_BUS_T4_ID])
    cg.add(var.set_parent(parent))