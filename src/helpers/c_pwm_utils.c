#include "c_pwm_utils.h"
uint64_t T_RPM_AC = 0, T_RPM_A = 0;
int _RPM = 0, _POS = 0;

uint_fast16_t grad_to_dnt(float grad) {
  return (uint_fast16_t)grad / (360 / DNT);
}

uint16_t dnt_to_grad(uint16_t _dnt) {
  return fast_mul_10((360 / DNT) * _dnt);
}

void RPM() {
  T_RPM_AC = millis();
  if (T_RPM_AC - T_RPM_A >= RPM_per) {
    T_RPM_A = T_RPM_AC;
    _RPM = (_POS / DNT) * 90;  // calculo para obtener las rpm
    _POS = 0;
  }
}

/*
  TODO: revisar si no mata mucho el rendimiento hacerlo en todos los ciclos (
  por las duda' ) this shit se ejecuta cada vez que no esta sincronizado el
  cigueñal, para bueno, sincronizarlo se exporta aca para no armar bardo el
  archivo principal en teoria: en la ecu anteriormente si el diente actual es +-
  60% mayor que el tiempo anterior es el diente doble = el proximo diente es el
  1, == ya ta sincroniza2
*/

bool sinc() {
#ifdef SINC_ENABLE
  if (!SINC) {
    if (sincB) {
      Tb = micros();
      T1 = Tb - Ta;
      Ta = Tb;
      sincB = false;
    } else {
      Tb = micros();
      T2 = Tb - Ta;
      Ta = Tb;
      sincB = true;
      return T2 > (T1 + (T1 / 2.3));
    }
    return false;
  }
  return true;
#else
  return true;
#endif
}
