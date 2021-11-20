#include "../include/tables.hpp"

extern "C"
{
#include "trace.h"
}
using namespace std;

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
    return (x + x + 1) + (y * x_max) + address;
}

int16_t tables::get_value(table_ref table, uint16_t x, uint16_t y)
{
    uint8_t data[3];
    uint32_t address = get_address(x, table.x_max, y, table.memory_address);

    W25qxx_ReadBytes(data, address, 2);

    // memory::read_multiple(address, data, 2);
    return ((int16_t)data[0] << 8) + data[1];
}

void tables::set_value(table_ref table, uint16_t x, uint16_t y, int16_t value)
{
    uint32_t address = get_address(x, table.x_max, y, table.memory_address);

    uint8_t data[2];

    data[0] = (value >> 8) & 0xFF;
    data[1] = value & 0xFF;

    W25qxx_WriteByte(data[0], address);
    W25qxx_WriteByte(data[1], address + 1);
    HAL_Delay(15);
}

TABLEDATA tables::read_all(table_ref table)
{

    TABLEDATA matrix(table.y_max, vector<int16_t>(table.x_max, 0xff));

    int row = 0;
    int datarow = 0;
    int column = 0;

    int rows = sizeof matrix / sizeof matrix[0]; // 2 rows

    int cols = sizeof matrix[0] / sizeof(int16_t); // 5 cols

    for (int16_t matrix_y = 0; matrix_y < table.y_max; matrix_y++)
    {
        uint32_t address = (column * table.x_max) + table.memory_address;
        uint8_t table_row[MAX_ROW_SIZE * 2];

        // W25qxx_ReadBytes(table_row, address, table.x_max * 2);

        for (int16_t matrix_x = 0; matrix_x < table.x_max; matrix_x++)
        {

            int16_t value = ((int16_t)table_row[datarow + 1] << 8) + table_row[datarow];
            value = tables::get_value(table, matrix_x, matrix_y);
            matrix[column][row] = value;

            auto volatile storageValue = matrix[column][row]; // DEBUG

            row += 1;
            datarow += 2;
        }

        column++;
        row = 0;
    }

    return matrix;
}

int16_t tables::find_nearest_neighbor(
    int16_t (&data)[MAX_ROW_SIZE], uint16_t data_size,
    int16_t search)
{
    double min_distance = 0xFFFF;
    int16_t min_distance_index = -1;

    for (uint16_t arr_index = 0; arr_index < data_size; arr_index++)
    {
        // Euclidean distance
        int16_t point = data[arr_index];
        double volatile distance = 0; // DEBUG
        distance = std::sqrt(std::exp2((double)point - search));

        // kNN 1D a lo pobre:
        if ((distance < min_distance && distance > 1) || (distance > min_distance && distance < 1))
        {
            min_distance = distance;
            min_distance_index = arr_index;
        }
    }

    if (min_distance_index > -1)
    {
        return data[min_distance_index];
    }
    return -1;
}
