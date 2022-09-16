#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <string>

#include "../webserial/commands.hpp"
#include "../webserial/commands_definition.hpp"
#include "defines.h"
#include "variables.h"

// RPM/PWM
#include "../cpwm/rpm_calc.h"

#define MEMORY_OFFSET 0xfffa;

namespace Engine {

void onEFISetup();
void onEFILoop();

// se ejecuta en cada vuelta fuera de los interrupts
void onEngineRev();

// parada del motor
void onEngineStop();

// aceleracion
void onEngineAcc();

// comandos usb comenzados en 66|67 corresponden a entrada/salida de comandos
// desde el frontend "Engine"
void onUSBCommand();

// helpers/internals
namespace sensors {
  enum _Sensors {
    COOLTANK_TEMP = 0,
    AIR_TEMP,
    MAP,
    TPS,
    O2,
  };
}
typedef sensors::_Sensors Sensors;
//  getSensor(Sensors::AIR_TEMP);

// returns current time on EFI
#define getEFITime() HAL_GetTick

// returns current RPM
#define getRPM() RPM::_RPM

// returns current cycle degree
#define getDEG() RPM::_DEG

// Sends Debug Message over USB (max 100 characters)
static inline void sendDebugMessage(std::string message) {

  uint8_t payload[123];
  strcpy((char *)payload, message.substr(0, 123).c_str());
  web_serial::queue_command(
      web_serial::create_command(FRONTEND_DEBUG_MESSAGE, payload));
}

// usb code 69 (nice)

// Sends Error Message over USB (max 100 characters)
static inline void sendErrorMessage(std::string message) {
  uint8_t payload[123];
  strcpy((char *)payload, message.substr(0, 123).c_str());
  web_serial::queue_command(
      web_serial::create_command(FRONTEND_ERROR_MESSAGE, payload));
}

// Overrides Injection time in uS, min 0uS, max 4000uS (4mS)
static inline void setInjectionTime(uint32_t injectionTime) {}

// Overrides Ignition Advance in degredees, max -35 , min 2
static inline void setIgnitionAdvance(float ignitionAdvance) {}

static inline void saveConfig(int32_t key, int32_t value) {}
static inline int32_t getConfig(int32_t key) {}

static inline int32_t getSensor(Sensors selectedSensor) {}
} // namespace Engine

#endif
