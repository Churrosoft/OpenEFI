#ifndef IAT_HPP
#define IAT_HPP

namespace IAT
{
    /** @brief devuelve valor en kPa del sensor
     * dependiendo las opciones de compilacion se agrega:
     * -healt check
     * @return valores de 0 a 260
     */
    uint16_t get_value(uint16_t);

    /** @brief devuelve valor mas exacto para calibracion inicial
     * dependiendo las opciones de compilacion se agrega:
     * -healt check
     * @return valores de -15 a 120
     */
    uint32_t get_calibrate_value(uint16_t);

    /** @brief revisa el estado del sensor para disparar un DTC en caso que sea necesario
     * @param in valor en Volt del sensor
     */
    uint8_t dtc(uint16_t);
} // namespace IAT

#endif