/* Esto se va a encargar de manejar el CD74HC4067 para obtener los diferentes sensores */

#include <stdint.h>
// libopencm3:
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
// custom:
#include "./ema_low_pass.c"
struct input_handler{
    /* data */
	struct EMALowPass values[16];// valores pasados por EMA Low Pass
} inputs;

/** devuelve input selecionada pasada por filtro EMA Low Pass
  * @param pin entrada analogica a leer
*/
uint16_t get_input(uint8_t);
/** devuelve data RAW del adc
*/
uint16_t get_adc_data(void);
/** inicia el ADC y los trigers por tiempo
*/
void input_setup(void);

uint16_t get_input(uint8_t pin){

   /*
   el sw para cambiar I/O del 74HC4067 para elegir el canal
   switch (pin){
    case 0:
		
        break;
    
    default:
        break;
    } */

	if(pin < 16){
		inputs.values[pin].actualValue = get_adc_data();
        inputs.values[pin] = EMALowPassFilter(inputs.values[0]);
		return inputs.values[pin].lastValue;
	}

	return 0;
}

void input_setup(){
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO14);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO15);
}

static void adc_setup(void){

	rcc_periph_clock_enable(RCC_ADC1);
	adc_power_off(ADC1); /* Make sure the ADC doesn't run during config. */
	adc_disable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);

	/* SOLO PARA TESTEAR, agarramos el sensor de temperatura integrado y usamos eso como entrada analogica */
	/* luego tendria que reemplazar esto por el canal analogico que vaya a usar*/
	adc_enable_temperature_sensor();

	// resto del setup del ADC
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);
	adc_power_on(ADC1);
	/* tiempo de espera del ADC, tanto tarda?, luego pasar a proceso asincrono de ultima*/
	for (uint16_t i = 0; i < 8000; i++) __asm__("nop");

	adc_reset_calibration(ADC1);
	adc_calibrate(ADC1);
}

uint16_t get_adc_data(){
	uint8_t channel_array[16];
	channel_array[0] = 16;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_direct(ADC1);
	// esperamos conversion y devolvemos el valor
	while (!adc_eoc(ADC1));
	uint16_t reg16 = adc_read_regular(ADC1);
	return reg16;
	//esto seria leyendo los registros a lo bruto, revisar si mejora el rendimiento
	/* while (!(ADC_SR(ADC1) & ADC_SR_EOC));
	return ADC_DR(ADC1); */
}