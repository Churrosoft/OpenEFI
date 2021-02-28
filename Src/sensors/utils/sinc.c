#include "./sinc.h"

uint32_t _POS;

/*-----( Variables sincronizado )-----*/
bool sincB;
bool initsinc;
unsigned long T1;
unsigned long T2;
unsigned long Ta;
unsigned long Tb;
bool SINC;
bool MOTOR_ENABLE;
// HAL:
extern TIM_HandleTypeDef htim3;

/** convierte grados en dientes del volante de inercia
 * @param grad grados a convertir
 * @return dientes equivalentes al angulo
*/ 
uint_fast16_t grad_to_dnt( float grad){
    return (uint_fast16_t) grad / (360 / DNT);
}


bool sinc() {
#ifdef SINC_ENABLE
  if(!MOTOR_ENABLE)
    return false;

  if (!SINC) {
    if (sincB) {
      Tb =  __HAL_TIM_GET_COUNTER(&htim3);
      T1 = Tb - Ta;
      Ta = Tb;
      sincB = false;
    } else {
      Tb =  __HAL_TIM_GET_COUNTER(&htim3);
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