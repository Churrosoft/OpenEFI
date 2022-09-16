#ifndef MEMORY_INMOBILIZER
#define MEMORY_INMOBILIZER

#include <stdint.h>
#include "memory_dtc.hpp"

/** @addtogroup Memory
 *  @brief Namespace con I/O hacia la memoria SPI externa
 * @{
 */

/** @addtogroup Immobilizer
 * @brief Implementacion de imobilizador primitivo, solo en casos de fallas
 * criticas
 * ## I N M O B I L I Z E R :
 * ahre porque el suspenso en el nombre \n
 * primer byte en primer pagina para indicar parada de emergencia (para no
 * arrancar el motor post reinicio de la misma), \n 6 bytes siguientes para el
 * DTC asociado: P0XXX => 6 char => 6 bytes se reserva 1/2 bloque, para
 * posterior \n uso como inmobilizador posta memory init addr: 0x2000 memory end
 * addr: 0x2420
 * @{
 */

/**
 * @brief lee la memoria y revisa por un DTC Critico que impida el arranque
 */
bool can_turn_on();

/**
 * @brief graba en la flash un dtc critico que impide el arranque en el proximo
 * ciclo
 */
void write_emergency_stop(dtc_data);

//! dir en memoria donde se ubica el flag de emergencia (1 bytes)
#define EMERGENCY_FLAG_ADDR 0x1000

//! dir en memoria donde se ubica el dtc que disparo la emergencia (6 bytes)
#define EMERGENCY_DTC_ADDR 0x1001

/*! @} End of Doxygen Immobilizer*/

/*! @} End of Doxygen Memory*/
#endif