#ifndef SENSORS_H
#define SENSORS_H

namespace sensors
{
    /** @brief Setup inicial de todos los sensores
    */
    static void setup(void);

    /** @brief Loop principal de los sensores, corre cada 250mS
      */
    static void loop(void);

} // namespace sensors

/* static void sensors::loop(){};

static void sensors::setup(){
    input_setup();
    adc_setup();
}
 */
//void sensors::loop(){};
#endif