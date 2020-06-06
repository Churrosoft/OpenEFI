#include "./input_handler.c"

/** @brief Setup inicial de todos los sensores
 *  */
void sensors_setup(void);

/** @brief Loop principal de los sensores, corre cada 250mS
 *  */
void sensors_loop(void);

void sensors_setup(){
    input_setup();
    adc_setup();
}