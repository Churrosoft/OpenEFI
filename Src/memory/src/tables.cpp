#include "../include/tables.hpp"

#include <stdio.h>

extern "C" {
#include "trace.h"
}
using namespace std;
#include "defines.h"

int32_t tables::get_value(table_ref table, uint32_t x, uint32_t y) {
  uint8_t data[3];
  uint32_t address = get_address(x, table.x_max, y, table.memory_address);

  W25qxx_ReadBytes(data, address, 2);

  return (int32_t)(data[1] << 8) + (data[2] << 16) + (data[3] << 24) + data[0];
}

void tables::set_value(table_ref table, uint32_t x, uint32_t y, int32_t value) {
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

table_data tables::read_all(table_ref table) {
  table_data matrix(table.y_max, vector<int32_t>(table.x_max, 0xff));

  uint32_t datarow = 0;

  for (int32_t matrix_y = 0; matrix_y < table.y_max; matrix_y++) {
    uint32_t address = (4 * matrix_y * table.x_max) + (W25qxx_SectorToPage(table.memory_address) * w25qxx.PageSize);

    uint8_t table_row[MAX_ROW_SIZE * 4];

    W25qxx_ReadBytes(table_row, address, table.x_max * 4);

    for (int32_t matrix_x = 0; matrix_x < table.x_max; matrix_x++) {
      volatile int32_t value =
          table_row[datarow] + (table_row[datarow + 1] << 8) + (table_row[datarow + 2] << 16) + (table_row[datarow + 3] << 24);

      matrix[matrix_y][matrix_x] = value;

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

bool tables::validate(table_ref table, table_data data) {
  uint32_t size = table.x_max * 4 * table.y_max;
  uint8_t *buffer = (uint8_t *)malloc(size);

  // load memory CRC
  uint8_t memory_crc_raw[4];
  uint32_t crc_address = (W25qxx_SectorToPage(table.memory_address) * w25qxx.PageSize) - 4;
  W25qxx_ReadBytes(memory_crc_raw, crc_address, 4);
  uint32_t memory_crc = (int32_t)(memory_crc_raw[1] << 8) + (memory_crc_raw[2] << 16) + (memory_crc_raw[3] << 24) + memory_crc_raw[0];

  // calculate buffer CRC
  dump_table(data, buffer);
  uint32_t table_crc = HAL_CRC_Calculate(&hcrc, (uint32_t *)buffer, size);

  free(buffer);

  return table_crc == memory_crc;
}

std::vector<int32_t> tables::put_row(uint8_t *data, uint32_t buff_size) {
  std::vector<int32_t> data_out;

  for (uint32_t index = 2; index < buff_size; index += 4) {
    int32_t row_value = (int32_t)(data[index + 1] << 8) + (data[index + 2] << 16) + (data[index + 3] << 24) + data[index];

    data_out.push_back(row_value);
  }
  return data_out;
}

void tables::update_table(table_data data, table_ref table) {
  int32_t size = table.x_max * 4 * table.y_max;
  uint8_t *buffer = (uint8_t *)malloc(size);

  dump_table(data, buffer);

  W25qxx_EraseSector(table.memory_address);

  W25qxx_WriteSector(buffer, table.memory_address, 0, size);

  free(buffer);
}

int32_t tables::find_nearest_neighbor(std::vector<int32_t> vec, int32_t search) {
  // "si anda, anda"
  auto upper_bound = std::upper_bound(vec.begin(), vec.end(), search, [](const int32_t &a, const int32_t &b) { return a <= b; });
  // std::upper_bound(vec.begin(), vec.end(), search);
  auto search_result = std::distance(vec.begin(), upper_bound);

  return (int32_t)std::abs(search_result);
}

void tables::plot_table(table_data table) {
  for (auto table_y : table) {
    char row[200];

    for (auto table_x : table_y) {
      sprintf(row, "%s [%4ld]", row, table_x);
    }
    trace_printf("%s\n", row);
    row[0] = 0;
  }
}

std::vector<int32_t> tables::col_to_row(table_data table, uint32_t table_index) {
  std::vector<int32_t> out_vec;

  for (auto table_y : table) {
    out_vec.push_back(table_y[table_index]);
  }
  return out_vec;
}

bool tables::on_bounds(table_ref table, int32_t x, int32_t y) {
  if (x > table.x_max || y > table.y_max) return false;

  if (x < 0 || y < 0) return false;

  return true;
}