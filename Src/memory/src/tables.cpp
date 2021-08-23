#include "../include/tables.hpp"

using namespace std;

int16_t tables::get_value(table_ref table, uint16_t x, uint16_t y)
{
    uint8_t data[2];
    uint32_t address = x + (y * table.x_max) + table.memory_address;
    memory::read_multiple(address, data, 2);
    return ((int16_t)data[0] << 8) + data[1];
}

void tables::set_value(table_ref table, uint16_t x, uint16_t y, int16_t value)
{
    uint32_t address = x + (y * table.x_max) + table.memory_address;
    uint8_t data[2];

    data[0] = (value >> 8) & 0xFF;
    data[1] = value & 0xFF;

    // table_ref myTable = {12, 12, 12};

    memory::write_multiple(address, data, 2);
}

TABLEDATA tables::read_all(table_ref table)
{

    TABLEDATA matrix(table.y_max, vector<int16_t>(table.x_max, 0xff));

    int row = 0;
    int datarow = 0;
    int column = 0;

    for (vector<int16_t> vect1D : matrix)
    {
        uint32_t address = (column * table.x_max) + table.memory_address;
        uint8_t table_row[MAX_ROW_SIZE * 2];

        memory::read_multiple(address, table_row, table.x_max * 2);

        for (int16_t x : vect1D)
        {

            int16_t value = ((int16_t)table_row[datarow] << 8) + table_row[datarow + 1];
            matrix[column][row] = value;

            auto storageValue = matrix[column][row]; // DEBUG

            row += 1;
            datarow += 2;
        }

        column++;
        row = 0;
    }

    return matrix;
}