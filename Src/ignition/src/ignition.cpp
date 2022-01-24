/*
Esto por ahora tiene que ser simple como el carajo, asi que:

Tabla avance inicial 12x12 => despues la estiramo hasta 24x24

sin ninguna correccion por sensores ni lambda

usando sensors => map y rpm nomas

*/

#include "../include/ignition.hpp"

extern "C" {
#include "trace.h"
}

TABLEDATA ignition::avc_tps_rpm;
bool ignition::loaded = false;
int32_t _AE = 0;

void ignition::interrupt() {
  if (!ignition::loaded)
    return;
  int32_t load = (int32_t)sensors::values._MAP;

  int16_t load_value = tables::find_nearest_neighbor(
      tables::col_to_row(ignition::avc_tps_rpm, 0), load);

  int16_t rpm_value =
      tables::find_nearest_neighbor(ignition::avc_tps_rpm[0], load);

  if (load_value < 17 && rpm_value < 17) {
    _AE = avc_tps_rpm.at(load_value).at(rpm_value);
  }
}

void ignition::setup() {
  table_ref ignition_table = TABLES_IGNITION_TPS_SETTINGS;
  ignition::avc_tps_rpm = tables::read_all(ignition_table);
  ignition::loaded = true;
}

/** Ejemplo tablita:
 * load(tps)/rpm
 * [  * ]  [550 ] [ 950] [1200] [1650] [2200] [2800] [3400] [3900] [4400] [4900]
 * [5400] [7200] [ 100]  (13.5) (13.0) (14.0) (14.0) (18.6) (24.0) (31.0) (33.0)
 * (33.2) (33.4) (33.6) (34.3) [ 90 ]  (13.8) (13.3) (14.0) (14.2) (17.4) (24.5)
 * (31.2) (33.3) (33.6) (33.8) (34.1) (34.9) [ 80 ]  (14.2) (13.6) (13.9) (14.4)
 * (17.8) (25.0) (31.5) (33.7) (34.0) (34.2) (34.5) (35.5) [ 70 ]  (14.5) (13.9)
 * (13.9) (14.6) (18.3) (25.5) (31.7) (34.0) (34.4) (34.7) (35.0) (36.1) [ 60 ]
 * (14.9) (14.2) (13.8) (14.8) (18.7) (25.9) (32.0) (34.4) (34.7) (35.1) (35.4)
 * (36.7) [ 50 ]  (15.3) (14.5) (13.8) (15.0) (19.1) (26.4) (32.2) (34.4) (35.1)
 * (35.5) (35.9) (37.3) [ 40 ]  (15.7) (14.8) (13.8) (15.2) (19.5) (26.9) (32.5)
 * (34.4) (35.5) (35.9) (36.4) (37.9) [ 30 ]  (16.1) (15.2) (14.4) (15.4) (19.9)
 * (27.4) (32.7) (34.4) (35.9) (36.4) (36.8) (38.5) [ 20 ]  (16.4) (15.5) (15.1)
 * (15.7) (20.4) (27.4) (34.9) (34.4) (36.3) (36.8) (37.3) (39.1) [ 15 ]  (16.6)
 * (15.7) (15.4) (15.8) (20.6) (28.3) (36.0) (34.4) (34.7) (37.0) (37.5) (39.4)
 * [ 10 ]  (16.8) (16.3) (15.7) (15.9) (20.8) (28.4) (36.0) (34.4) (34.7) (37.2)
 * (37.8) (39.7) [  5 ]  (17.0) (16.5) (16.0) (16.0) (21.0) (28.5) (36.0) (34.4)
 * (34.7) (37.4) (38.0) (40.0)
 */
/**
 *
 *  * [ 20 ]  164, 155, 151, 157, 204, 274, 349, 344, 363, 368, 373, 391,
 * [ 15 ]  166, 157, 154, 158, 206, 283, 360, 344, 347, 370, 375, 394,
 * [ 10 ]  168, 163, 157, 159, 208, 284, 360, 344, 347, 372, 378, 397,
 * [  5 ]  170, 165, 160, 160, 210, 285, 360, 344, 347, 374, 380, 400,
 */
