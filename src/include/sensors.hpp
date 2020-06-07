#ifndef SENSORS_H
#define SENSORS_H
//#include "sensors.cpp"
namespace sensors
{
    /** @brief Setup inicial de todos los sensores
    */
     void setup(void);

    /** @brief Loop principal de los sensores, corre cada 250mS
      */
     void loop(void);

} // namespace sensors

/* static void sensors::loop(){};

static void sensors::setup(){
    input_setup();
    adc_setup();
}
 */
//void sensors::loop(){};
#endif