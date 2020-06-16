#ifndef SENSORS_CPP
#define SENSORS_CPP

namespace sensors
{
    /** @brief Setup inicial de todos los sensores
    */
    static void setup(void);

    /** @brief Loop principal de los sensores, corre cada 250mS
      */
    static void loop(void);

} // namespace sensors

static void sensors::loop(){

};

static void sensors::setup(){
    //input_setup();
    //adc_setup();
}

#endif