#include "../include/tables.hpp"

extern "C" {
#include "trace.h"
}
using namespace std;
#include "defines.h"

int32_t tables::get_value(table_ref table, uint16_t x, uint16_t y) {
  uint8_t data[3];
  uint32_t address = get_address(x, table.x_max, y, table.memory_address);

  W25qxx_ReadBytes(data, address, 2);

  return (int32_t)(data[1] << 8) + (data[2] << 16) + (data[3] << 24) + data[0];
}

void tables::set_value(table_ref table, uint16_t x, uint16_t y, int16_t value) {
  uint32_t address = get_address(x, table.x_max, y, table.memory_address);

  uint8_t data[4];

  /*     data[0] = (value >> 8) & 0xFF;
  data[1] = value & 0xFF; */

  data[0] = (uint8_t)value;
  data[1] = (uint8_t)(value >> 8) & 0xFF;
  data[2] = (uint8_t)(value >> 16) & 0xFF;
  data[3] = (uint8_t)(value >> 24) & 0xFF;

  W25qxx_WriteByte(data[0], address);
  W25qxx_WriteByte(data[1], address + 1);
  W25qxx_WriteByte(data[2], address + 2);
  W25qxx_WriteByte(data[3], address + 3);

  HAL_Delay(15);
}

TABLEDATA tables::read_all(table_ref table) {

  TABLEDATA matrix(table.y_max, vector<int32_t>(table.x_max, 0xff));

  uint32_t datarow = 0;

  for (int32_t matrix_y = 0; matrix_y < table.y_max; matrix_y++) {

    uint32_t address =
        (4 * matrix_y * table.x_max) +
        (W25qxx_SectorToPage(table.memory_address) * w25qxx.PageSize);

    uint8_t table_row[MAX_ROW_SIZE * 4];

    W25qxx_ReadBytes(table_row, address, table.x_max * 4);

    for (int32_t matrix_x = 0; matrix_x < table.x_max; matrix_x++) {

      volatile int32_t value =
          table_row[datarow] + (table_row[datarow + 1] << 8) +
          (table_row[datarow + 2] << 16) + (table_row[datarow + 3] << 24);

      matrix[matrix_y][matrix_x] = value;

      auto volatile storageValue = matrix[matrix_y][matrix_x]; // DEBUG
      BREAKPOINT
      datarow += 4;
    }
    datarow = 0;
  }

  return matrix;
}

void tables::dump_row(std::vector<int32_t> table_row, uint8_t *dest_buff) {
  uint32_t index = 0;
  for (int32_t table_x : table_row) {
    dest_buff[index] = (uint8_t)table_x;
    dest_buff[index + 1] = (uint8_t)(table_x >> 8) & 0xFF;
    dest_buff[index + 2] = (uint8_t)(table_x >> 16) & 0xFF;
    dest_buff[index + 3] = (uint8_t)(table_x >> 24) & 0xFF;

    index += 4;
  }
}
std::vector<int32_t> tables::put_row(uint8_t *data, uint32_t buff_size) {
  std::vector<int32_t> data_out;

  for (uint32_t index = 2; index < buff_size; index += 4) {
    data_out.push_back((int32_t)(data[index + 1] << 8) +
                       (data[index + 2] << 16) + (data[index + 3] << 24) +
                       data[index]);
  }
  return data_out;
}

void tables::update_table(TABLEDATA data, table_ref table) {
  int32_t size = table.x_max * 4 * table.y_max;
  uint8_t *buffer = (uint8_t *)malloc(size);

  dump_table(data, buffer);
  uint8_t a = buffer[0];
  uint8_t b = buffer[1];
  uint8_t c = buffer[2];
  uint8_t d = buffer[3];
  W25qxx_EraseSector(table.memory_address);

  W25qxx_WriteSector(buffer, table.memory_address, 0, size);

  BREAKPOINT
  free(buffer);
}

int16_t tables::find_nearest_neighbor(std::vector<int32_t> vec,
                                      int32_t search) {

  int16_t search_result = std::abs(std::distance(
      vec.begin(), std::upper_bound(vec.begin(), vec.end(), search)));

  if (search_result == *vec.end()) {
    return -1;
  }
  return search_result;
}

void tables::plot_table(TABLEDATA table) {
  for (auto table_y : table) {
    char row[200];

    for (auto table_x : table_y) {
      sprintf(row, "%s [%4ld]", row, table_x);
    }
    trace_printf("%s\n", row);
    row[0] = 0;
  }
}

std::vector<int32_t> tables::col_to_row(TABLEDATA table, uint16_t table_index) {
  std::vector<int32_t> out_vec;

  for (auto table_y : table) {
    out_vec.push_back(table_y[table_index]);
  }
  return out_vec;
}