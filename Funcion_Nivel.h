/*******************************************************************************
 * MÓDULO DE SENSOR DE NIVEL DE AGUA
 *
 * Sensor: XKC-Y25-V (capacitivo, sin contacto, detecta a través del cristal)
 *   - Marrón: VCC | Amarillo: salida | Azul: GND | Negro: MODO
 *   - Negro al aire: salida en ALTO cuando detecta agua
 *   - Negro unido al azul: lógica invertida. NUNCA usar el negro como GND
 *
 * ⚠ PENDIENTE DE HARDWARE: el XKC-Y25-V necesita 5-24 V de alimentación y su
 * salida en ALTO es igual a VIN. Alimentado a 5 V entrega 5 V al GPIO, y el
 * ESP32 admite como máximo 3,6 V. Añadir un divisor 10 kΩ (serie) / 20 kΩ
 * (a GND) en el cable amarillo, o cambiar al modelo XKC-Y25-NPN. Ver README.
 *******************************************************************************/

#ifndef FUNCION_NIVEL_H_
#define FUNCION_NIVEL_H_

/************************* DEFINICIÓN DE CONSTANTES **************************/
#define LEVEL_SENSOR 25               // GPIO25 conectado a la salida del sensor
#define NIVEL_ACTIVO_ALTO true        // true: ALTO = agua detectada (negro al aire)

// El oleaje del filtro hace oscilar la salida cuando el agua roza el sensor.
// Un cambio solo se acepta si se mantiene estable durante este tiempo.
#define NIVEL_TIEMPO_ESTABLE_MS 5000

/**
 * Configura el pin del sensor y toma la lectura inicial
 */
void setupSensorNivel();

/**
 * Lee el sensor y aplica el filtro antirrebote.
 * Debe llamarse en cada vuelta de loop(); no bloquea.
 */
void actualizarSensorNivel();

/**
 * @return true si el nivel de agua es correcto, false si está bajo
 */
bool nivelCorrecto();

#endif // FUNCION_NIVEL_H_
