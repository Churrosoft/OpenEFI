
#include "../include/config.hpp"

#include <cstring>

#include "defines.h"
#include "efi_config.hpp"

extern "C" {
#include "trace.h"
}

static_assert(sizeof(engine_config) < 250, "Struct de configuracion demasiado grande, refactorizar codigo de engine_config");

engine_config efi_cfg::get() {
  uint8_t pdata[256];
  engine_config cfg;
  uint8_t engine_config_size = sizeof(engine_config);

  W25qxx_ReadPage(pdata, 0, 0, 256);
  uint32_t memory_crc = (uint32_t)(pdata[253] << 8) + (pdata[254] << 16) + (pdata[255] << 24) + pdata[252];

  std::memcpy(&cfg, pdata, engine_config_size);
  uint32_t readed_crc = CrcCCITTBytes(pdata, engine_config_size);

  if (memory_crc == readed_crc) {
    return cfg;
  }
  // la data no coincide por estar corrupta/cambio el struct, devuelta de 0 to2:
  trace_printf("Event: <CONFIG CRC HEX> Calculated: %d %d %d %d ## Stored: %d %d %d %d\r\n", (uint8_t)readed_crc,
               (uint8_t)(readed_crc >> 8) & 0xFF, (uint8_t)(readed_crc >> 16) & 0xFF, (uint8_t)(readed_crc >> 24) & 0xFF, pdata[252],
               pdata[253], pdata[254], pdata[255]);

  efi_cfg::set_default();
  efi_cfg::set(efi_config);

  return cfg;
}

void efi_cfg::set(engine_config newcfg) {
  uint8_t pdata[256];
  uint8_t engine_config_size = sizeof(engine_config);

  std::memcpy(pdata, &newcfg, engine_config_size);

  uint32_t new_crc = CrcCCITTBytes(pdata, engine_config_size);

  pdata[252] = (uint8_t)new_crc;
  pdata[253] = (uint8_t)(new_crc >> 8) & 0xFF;
  pdata[254] = (uint8_t)(new_crc >> 16) & 0xFF;
  pdata[255] = (uint8_t)(new_crc >> 24) & 0xFF;
  pdata[256] = 0;

  W25qxx_WritePage(pdata, 0, 0, 256);

  trace_printf("Event: <NEW CONFIG CRC HEX> Calculated: %d %d %d %d ##", pdata[252], pdata[253], pdata[254], pdata[255]);
}
