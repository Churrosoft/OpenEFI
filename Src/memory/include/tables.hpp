/**
 * Tablitas que mas va a ser:
 * get_value(TABLE_REF, X, Y);
 * set_value(TABLE_REF, X, Y, value);
 * todos los valores que se graban en la memoria son int16
 * el "TABLE_REF" va a ser un struct con el tamaño en X/Y, y el address de inicio en la memoria
 * /
 * 
 */
#include <stdint.h>

#ifndef TABLES_HPP
#define TABLES_HPP

extern "C"
{
#include "w25qxx.h"
}

#include "memory.hpp"
#include <limits.h>
#include <math.h>
#include <vector>

#define TABLEDATA std::vector<std::vector<int16_t>>
#define MAX_ROW_SIZE 30

struct table_ref
{
    uint16_t y_max;
    uint16_t x_max;
    uint16_t memory_address;
};

/*!
 *  addtogroup Tables
 *  @{
 */

//! Implementacion de tablas 2D con vectores, valores en uint16_t
namespace tables
{
    // operaciones simples sobre un campo:
    int16_t get_value(table_ref, uint16_t, uint16_t);
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

    // utils para manejar data de las tablas:
    int16_t find_nearest_neighbor(int16_t (&)[MAX_ROW_SIZE], uint16_t, int16_t);

    namespace
    {
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
        static inline uint32_t get_address(uint16_t x, uint16_t x_max, uint16_t y, int16_t address)
        {
            return (x + x + 1) + (y + y * x_max) + address;
        }
    }
} // namespace tables

/*! @} End of Doxygen Tables*/


#endif