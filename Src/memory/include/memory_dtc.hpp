#ifndef MEMORY_DTC
#define MEMORY_DTC


/** @addtogroup Memory
 *  @brief Namespace con I/O hacia la memoria SPI externa
 * @{
 */

/** @addtogroup DTC_IO
 * @brief almacenaje/borrado de DTC's en memoria

 * DTC Example: \n
 * primer byte en primer pagina podria indicar si hay o no dtcs grabados \n
 * segundo byte cantidad total de DTC guardados \n
 * P0XXX => 6 char => 6 bytes (2 byte adicional para indicar si es temporal | permanente / cantidad?) \n
 * P0XXX{5 veces}{1 temporal} \n
 * max dtc on memory => 50 => 200 bytes \n
 * memory init addr: 0x1000 \n
 * memory end addr:  0x1FFF \n

 * @{
 */
#include <stdint.h>
#include <vector>

extern "C" {
#include "w25qxx.h"
}


#include <initializer_list>
#include "dtc_codes.h"

std::vector<std::vector<uint8_t>>get_all_dtc();
// void write_dtc(std::initializer_list<uint8_t *>, uint8_t);

void write_dtc(uint8_t *);
void read_dtc(uint8_t, uint8_t *);
void write_dtc(uint8_t *,uint8_t);
void read_dtc(uint8_t, uint8_t *);

#define DTC_FLAG_ADDR 0x1000

#define DTC_INIT_PAGE 20
#define DTC_END_ADDR 0x1FFF
#define DTC_EMPTY 0xFE
#define DTC_EMPTY_ALT 0xFF


/*! @} End of Doxygen DTC_IO*/

/*! @} End of Doxygen Memory*/

#endif