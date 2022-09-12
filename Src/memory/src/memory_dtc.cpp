#include "../include/memory_dtc.hpp"

using namespace std;

std::vector<dtc_data> get_all_dtc() {
  // lee la memoria buscando la cantidad de dtc's existentes
  std::vector<dtc_data> dtc_list;
  uint8_t buff[300];
  W25qxx_ReadPage(buff, DTC_INIT_PAGE, 0, 0);
  for (uint16_t index = 1; index < 31; index++) {
    // si el ultimo index del dtc no es 0xFE que se usa para marcar los
    // invalidos, atroden
    if (buff[index * 8 + 8] != 0xFE) {
      dtc_data dtc;
      dtc.push_back(buff[index * 8]);
      dtc.push_back(buff[index * 8 + 1]);
      dtc.push_back(buff[index * 8 + 2]);
      dtc.push_back(buff[index * 8 + 3]);
      dtc.push_back(buff[index * 8 + 4]);
      dtc.push_back(buff[index * 8 + 5]);
      dtc.push_back(buff[index * 8 + 6]);
      dtc.push_back(buff[index * 8 + 7]);
      dtc_list.push_back(dtc);
    }
  }
  return dtc_list;
}

bool is_dtc() {
  // devuelve true en caso de que exista un dtc;
  /*     uint16_t data = memory::read_single(DTC_FLAG_ADDR);
      return data != 0xFE; */
  return false;
}

void write_dtc(dtc_data dtc_code) {
  // cada pagina tiene 256 bytes, 31 dtc unicos (que ma' quere)
  // PAGE + 1 => cantidad DTC
  // PAGE + 2 => inmobilizador (dtc critico)

  /*
  uint16_t dtc_number = get_all_dtc();
  if (dtc_number && dtc_number < 5)
  {
      memory::write_multiple(DTC_INIT_ADDR + ((dtc_number + 1) * 5), dtc_code,
  5);
      //  memory::write_single(DTC_FLAG_ADDR, 0);
  }
  else
  {
      memory::write_multiple(DTC_INIT_ADDR, dtc_code, 5);
      // memory::write_single(DTC_FLAG_ADDR, 0);
  } */
}

// busca un codigo en la memoria para ver si ya existe, (no vamo a grabar las
// cosas dos veces vite')
bool search_dtc(uint8_t *dtc_code) { return false; }
