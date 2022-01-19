/** @file */
#include <stdint.h>

#ifndef CHT_1_6_H
#define CHT_1_6_H

#undef mtr
#undef CIL
#undef L_CIL
#undef DNT
#undef DNT_MISSING
#undef LOGIC_DNT
#undef Alpha
#undef ED
#undef PMSI
#undef AVCI
#undef ECNT

/*-----( Globales )-----*/

#define mtr 1           //!< habilita encendido
#define CIL 4           //!< cantidad de cilindros o pistones, o camaras de combustion, etc ?)
#define L_CIL (CIL - 1) //!< cilindros logicos, para manejar arrays y demases
#define DNT 60          //!< cantidad de dientes de la rueda fonica
#define DNT_MISSING 2   //!< cantidad de dientes faltantes en PMS
#define LOGIC_DNT (DNT - DNT_MISSING)
#define Alpha 1         //!< modo para probar sin correcciones de tiempo, ni algoritmos de inyeccion ni sincronizacion, para encajar un 555 y probar a pelo ?)
#define ED 1600         //!< cilindrada en CC del motor
#define MAX_RPM 6500
/*-----( C_PWM )-----*/

#define PMSI 5 //!< Cantidad de dientes entre PMS

// Inyeción:
#define AVCI 30 //avance de inyeccion (si queres quemar las valvulas dejalo en 0)

// Encendido:
#define ECNT 20 //tiempo en ms que se prende la bobina (cambie a su riesgo)

#endif