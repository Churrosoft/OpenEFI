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

extern "C"{
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

namespace tables
{
    // operaciones simples sobre un campo:
    int16_t get_value(table_ref, uint16_t, uint16_t);
    void set_value(table_ref, uint16_t, uint16_t, int16_t);
    int32_t get_long_value(table_ref, uint16_t, uint16_t);
    void set_long_value(table_ref, uint16_t, uint16_t, int32_t);
    // operaciones sobre varios campos a la vez:
    TABLEDATA read_all(table_ref);
    // utils para manejar data de las tablas:
    int16_t find_nearest_neighbor(int16_t (&)[MAX_ROW_SIZE],uint16_t, int16_t);
} // namespace tables

#endif