#ifndef CPWM_UTILS
#define CPWM_UTILS

#include "../defines.h"
#include <stdint.h>

/** convierte grados en dientes del volante de inercia
 * @param grad grados a convertir
 * @return dientes equivalentes al angulo
*/ 
uint_fast16_t grad_to_dnt( uint16_t );

//convierte grados en dientes del sensor hall
int dientes(float grados){
    float grad = 360 / DNT; //dividimos 360 grados por la cantidad de dientes
    //dividimos grados por grad, luego multiplicamos por 100 para transformar el float en int
    int x2 = (grados / (360 / DNT)) * 100;
    //dividimos por 100, al hacer esto se eliminan los decimales, en prox ver redondear
    int dnt2 = x2 / 100;
    return dnt2;
}

uint_fast16_t grad_to_dnt( uint16_t grad){
    return (uint_fast16_t) grad / (360 / DNT);
}
// OLD code:
/*
  float grad = 360 / DNT; //dividimos 360 grados por la cantidad de dientes
  //dividimos grados por grad, luego multiplicamos por 100 para transformar el float en int
  int x2 = (grados / grad) * 100;
  //dividimos por 100, al hacer esto se eliminan los decimales, en prox ver redondear
  int dnt2 = x2 / 100;
  return dnt2;
*/
#endif