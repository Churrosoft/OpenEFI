/*
Esto por ahora tiene que ser simple como el carajo, asi que:

Tabla avance inicial 12x12 => despues la estiramo hasta 24x24

sin ninguna correccion por sensores ni lambda

usando sensors => map y rpm nomas

*/

#include "../include/ignition.hpp"
TABLEDATA avc_tps_rpm;

void ignition::interrupt()
{
}

void ignition::setup()
{
    table_ref ignition_table = {13, 13, 0x2000};
    avc_tps_rpm = tables::read_all(ignition_table);
}

/** Ejemplo tablita:
 * rpm/load
 * [  *   ]  [10] [20] [30] [40] [50]
 * [ 500  ]  (12) (13) (15) (07) (05)
 * [ 750  ]
 * [ 1000 ]
 * [ 1500 ]
 * [ 2000 ]
 * [ 2500 ]
*/