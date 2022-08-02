// PONELE QUE ENTENDI ALGO DE TANTO PDF:
// codigo para soportar OBD-2 para scanners genericos siguiendo estandar:
// Layer 7 SAE J1979/ISO 15031-5
// Layer 5/6 ISO 15765-4:2021
// Layer 2 ISO 15765-2:2016
// Layer 1-2 ISO 9141-2

#include "dtc_codes.h"
#include "obd2_pid.h"
#include "obd2_status.h"
#include <endian.h>

namespace OBD2 {

void write_dtc_to_mem();
void parse_can_message();
void queue_can_message();

void loop(void);
namespace {
  void create_can_message();
}
} // namespace OBD2
