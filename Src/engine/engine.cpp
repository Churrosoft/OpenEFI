// Este archivo se encarga de todas las tareas comunes del motor de cara al
// usuario (alg customs de inyeccion/ aceleracion / etc)
#include "engine.hpp"

using namespace Engine;

void Engine::onEFILoop() {
  //
}

void Engine::onEFISetup() {
  //
  int32_t sensor_value = getSensor(Sensors::AIR_TEMP);
  int32_t rpm_actual = getRPM();

  setInjectionTime(420);
  setIgnitionAdvance(69.420);
  
  EFI_LOG("debug con sin char[] para que no se nismee nadie");
  EFI_ERROR("ooops revento algo");
}


