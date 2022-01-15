/** @file */
#include <stdint.h>
#include "main.h"

// Acá todos los defines
#ifndef DEFINES_H
#define DEFINES_H

// Version de firmware
#define OPENEFI_BOARD_TYPE 1 // 0 OpenEFI; 1 uEFI, 2 DashDash
#define OPENEFI_VER_MAJOR 2
#define OPENEFI_VER_MINOR 1
#define OPENEFI_VER_REV 1

/*-----( Globales )-----*/

#define mtr 1             //!< habilita encendido
#define CIL 4             //!< cantidad de cilindros o pistones, o camaras de combustion, etc ?)
#define L_CIL (CIL - 1)   //!< cilindros logicos, para manejar arrays y demases
#define DNT 60            //!< cantidad de dientes del sensor CKP
#define DNT_MISSING 0     //!< cantidad de dientes faltantes en PMS
#define DNT_DOUBLE_SCAN 2 //!< 1 == interrupcion por diente
#define LOGIC_DNT ((DNT - DNT_MISSING) * DNT_DOUBLE_SCAN)
#define Alpha 1                    //!< modo para probar sin correcciones de tiempo, ni algoritmos de inyeccion ni sincronizacion, para encajar un 555 y probar a pelo ?)
#define ED 1600                    //!< cilindrada en CC del motor
#define ING_SECUENCY {1, 3, 4, 2}; // secuencia encendido
#define INY_SECUENCY {3, 4, 1, 2}; // secuencia inyeccion
#define MAX_RPM 4500 //!< valor maximo de rpm, valores superiores activan cutoff de encendido/combustible

/*-----( RPM )-----*/

#define RPM_per 500 //periodo en ms en el que se actualizan las rpm ( si lo cambias , o arreglas el calculo para las rpm,o se rompe todo maquinola)

/*-----( C_PWM )-----*/

#define CPWM_ENABLE //!< Habilita el control de PWM
#define SINC_ENABLE //!< No borre este define, no sea tarado

#define PMSI 240 //!< Cantidad de dientes entre PMS

// Inyeción:
#define AVCPER 360 / (CIL / 2)
#define AVCI 30 //avance de inyeccion (si queres quemar las valvulas dejalo en 0)

// Encendido:
#define ECNT 20 //tiempo en ms que se prende la bobina (cambie a su riesgo)

/*-----( I_TIME )-----*/
/* n = PV / RT
In order to use n = PV / RT to calculate the amount of air a motor ingests during the induction stroke we would need:

P is pressure in the cylinder immediately after the intake valves close.
V is volume, which we know from engine displacement.
R we know (it’s the Ideal Gas Constant)
T is the temperature of the gas in the cylinder immediately after the intake valves close.
*/

#define _R 8.31446261815324 // en Pascal
#define _N_CALC(KPa, iat_temp) (((KPa * 1000) * ED) / (_R * iat_temp))

/*-----( I_IALG )-----*/
//cuando tenga la libreria de memoria paso todo a variable, por ahora con define
//int INY_L = 150,   //tiempo de apertura del inyector en microsegundos
//INY_P = 500,   //tiempo en uS adicional para acelerar el motor
//INY_C = 25000; //Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiom�trica (lambda=1 ) , para cilindrada del motor , presi�n a 100kPa , temperatura del aire a 21�C y VE 100% .

#define INY_C 8000
#define INY_P 500
#define INY_L 150
#define ARRTp 45   //temperatura para definir si es arranque en frio o en caliente
#define TARR 3500  //tiempo de inyeccion para arranque (frio)
#define TARR2 2500 //tiempo de inyeccion para arranque (caliente)

/*-----( Variables _LMB )-----*/

#define LMBM false //en true si se utilizan las dos sondas lambda
#define CTA 250    //Correcion de tiempo A, mezcla rica, se le sacan X uS
#define CTB 300    //Correcion de tiempo B, mezcla pobre, se le agregan X uS
#define P_LMB 250  //periodo en mS en el que se corrije por sonda lamba
#define T_LMB 45   //temperatura a partir de la cual se intenta correjir el tiempo de inyeccion
#define FLMBA 1.5  //factor maximo de lambda
#define FLMBB 0.85 //factor minimo de lambda

/*-----( Variables BPW )-----*/

#define BPC 1500 //Base Pulse Constant
#define AF 123   // relacion aire combustible, se divide por 10 para no usar float
#define BVC 1    //correcion por bateria, luego intentar usar tabla
#define BLM 168  // "Block Learn" varia entre 168 y 170 dependiendo de como resultaron los tiempos
//anteriores, seudo IA para mejorar el tiempo de inyeccion
#define DFCO 1 //ni puta idea
#define DE 1   // ni puta idea x2
#define TBM 1  //turbo boost multiplier DEJAR EN 1 CARAJO, que sino rompes todo

/*-----( Sensors )-----*/

//  ADC:
// R2 = R1 * (1 / ((Vref / Vin) - 1)
#define R1 10000           //!< para testear ahora todos los sensores tienen una resitencia de 10k en el divisor resistivo
#define Vref 3300          //!< Volts del Vref del ADC, luego se puede reemplazar por el registro para que sea mas exacto
#define ADC_MAX_VALUE 4095 //!< valor maximo del ADC, a 12 bit es 4095

//  TPS:
#define TPS_DUAL false //!< Habilita el TPS doble, requiere remapeo de IO

#define TPS_MIN_A 1700 //!< Valor minimo en mV para el sensor TPS (primer potenciometro); valores inferiores disparan DTC
#define TPS_MIN_B 1700 //!< Valor minimo en mV para el sensor TPS (segundo potenciometro); valores inferiores disparan DTC
#define TPS_MAX_A 4500 //!< Valor maximo en mV para el sensor TPS (primer potenciometro); valores superiores disparan DTC
#define TPS_MAX_B 4500 //!< Valor maximo en mv para el sensor TPS (segundo potenciometro); valores superiores disparan DTC

#define TPS_CALC_A(mV) ((mV <= 3695) ? (mV - 1823) / 37.44 : (mV - 2943) / 15.04) //!< Ecuacion para transformar valor en mV a porcentaje (varia dependiendo del sensor); NO MANDES UN MAP() DE ARDUINO ACA

//  TEMP:
#define TEMP_MIN 800
#define TEMP_MAX 4800
#define TEMP_OPEN 600
#define TEMP_MAX_VALUE 13000 // 130.00°C

//float C = 2.019202697e-07; // Modelo B de NTC
#define TEMP_K 2.5 //factor de disipacion en mW/C

//  MAP:
#define MAP_MIN 800
#define MAP_MAX 4800

#define MAP_CAL(mV) (mV * 16.66 + 167)

//  IAT:
#define IAT_MIN 200
#define IAT_MAX 4000
#define IAT_OPEN 190

#define IAT_CAL(mV) ((3800 - mV) / 35)

/*-----( Tables )-----*/

#define TABLES_IGNITION_TPS_SETTINGS { 17, 17, 0x2 }

/*-----( Helpers )-----*/

#define ROUND_16(NUMBER) ((float)((uint16_t)NUMBER * 100 + .5) / 100)
#define ROUND_32(NUMBER) ((float)((uint32_t)NUMBER * 100 + .5) / 100)

// GET_BIT only for uint8_t
#define GET_BIT(VAR, BIT_NEEDED) ((VAR >> BIT_NEEDED) & 1) //(VAR & (1 << BIT_NEEDED)) // another way: ((VAR >> BIT_NEEDED) & 1)

/*-----( pa' el debug )-----*/

#define CPWM_DEBUG 1
#define PMIC_DEBUG 1

#define BREAKPOINT void();

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 
  
#endif