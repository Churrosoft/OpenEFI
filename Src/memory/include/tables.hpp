/**
 * Tablitas que mas va a ser:
 * get_value(TABLE_REF, X, Y);
 * set_value(TABLE_REF, X, Y, value);
 * todos los valores que se graban en la memoria son int16
 * el "TABLE_REF" va a ser un struct con el tamaño en X/Y, y el address de
 * inicio en la memoria
 * /
 *
 */
#include <stdint.h>

#ifndef TABLES_HPP
#define TABLES_HPP

extern "C" {
#include "w25qxx.h"
}

#include "memory.hpp"
#include <algorithm>
#include <limits.h>
#include <math.h>
#include <vector>

#define TABLEDATA std::vector<std::vector<int32_t>>
#define MAX_ROW_SIZE 30

struct table_ref {
  uint16_t y_max;
  uint16_t x_max;
  int32_t memory_address;
};

/** @addtogroup Tables
 * @{
 */

#define MEMORY_SECTOR_SIZE 4096
#define MEMORY_SECTOR_COUNT 4096
#define MEMORY_PAGE_SIZE 256
#define SECTOR_TO_ADDRES(sector) (sector * 16 * MEMORY_PAGE_SIZE - 1)

//! Implementacion de tablas 2D con vectores, valores en uint16_t
namespace tables {
// operaciones simples sobre un campo:
int32_t get_value(table_ref, uint16_t, uint16_t);
void set_value(table_ref, uint16_t, uint16_t, int16_t);
int32_t get_long_value(table_ref, uint16_t, uint16_t);
void set_long_value(table_ref, uint16_t, uint16_t, int32_t);

// operaciones sobre varios campos a la vez:

/**
 * @brief reads all data of the selected table
 * @param {table_ref} table - table to read
 * @return {TABLE_DATA} - vector 2D with uint16_t data
 */
TABLEDATA read_all(table_ref);

// TABLEDATA alter_table(TABLEDATA, uint16_t, uint16_t, uint16_t);

// utils para manejar data de las tablas:
int32_t find_nearest_neighbor(std::vector<int32_t>, int32_t);

/**
 * @brief erases page on memory and record new data of table
 */
void update_table(TABLEDATA, table_ref);

void dump_row(std::vector<int32_t>, uint8_t *);

std::vector<int32_t> put_row(uint8_t *, uint32_t);

// debug:
void plot_table(TABLEDATA);
namespace {
  /***
   * @brief Retorna direccion de memoria para la posicion 2D de la tabla
   * @param x eje X de la tabla
   * @param y eje Y de la tabla
   * @param x_max configuracion de la tabla, valor maximo que toma el eje X
   * @param address direccion inicial de la tabla (x/y = 0)
   * @example  solo 2 valores en X, 2 en Y
   * address: [336] [337] # [338]  [339] | [340]  [341] #  [342] [343]
   *    X:      0     0   #  1      1    |  0       0   #    1     1
   *    Y:      0     0   #  0      0    |  1       1   #    1     1
   */
  static inline uint32_t get_address(uint16_t x, uint16_t x_max, uint16_t y,
                                     int32_t address) {
    return 4 * x + (4 * y * x_max) + address;
  }

  /**
   * @brief dump all data to uint8_t array
   * @param {TABLE_DATA} - table to dump
   * @param {uint8_t *} - destination array
   */
  static inline void dump_table(TABLEDATA table, uint8_t *dest_arr) {
    uint16_t index = 0;
    for (auto table_y : table) {
      // 312
      for (int32_t table_x : table_y) {
        dest_arr[index] = (uint8_t)table_x;
        dest_arr[index + 1] = (uint8_t)(table_x >> 8) & 0xFF;
        dest_arr[index + 2] = (uint8_t)(table_x >> 16) & 0xFF;
        dest_arr[index + 3] = (uint8_t)(table_x >> 24) & 0xFF;

        /*                     dest_arr[index] = (table_x >> 8) & 0xFF;
        dest_arr[index + 1] = table_x & 0xFF; */
        index += 4;
      }
    }
  }
} // namespace
} // namespace tables

/*! @} End of Doxygen Tables*/

#endif