#ifndef MEMORY_INMOBILIZER
#define MEMORY_INMOBILIZER

/**
 * I N M O B I L I Z E R : ahre porque el suspenso en el nombre
 * primer byte en primer pagina para indicar parada de emergencia (para no arrancar el motor post reinicio de la misma),
 * 6 bytes siguientes para el DTC asociado:
 * P0XXX => 6 char => 6 bytes
 * se reserva 1/2 bloque, para posterior uso como inmobilizador posta
 * memory init addr: 0x2000
 * memory end addr:  0x2420
 */

#include <stdint.h>

bool can_turn_on();
void write_emergency_stop(uint8_t *);

#define EMERGENCY_FLAG_ADDR 0x1000
#define EMERGENCY_DTC_ADDR  0x1001
#endif