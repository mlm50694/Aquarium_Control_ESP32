/*******************************************************************************
 * MÓDULO DE MEDIDA DE LA PILA DEL COMEDERO AUTOMÁTICO
 *
 * Mide la tensión de la pila mediante un divisor resistivo conectado a una
 * entrada ADC1. El divisor reduce la tensión de la pila por debajo de 3,3 V.
 *
 *   Pila (+) ── R_SUPERIOR ──┬── BATERIA_PIN
 *                            ├── 100 nF ── GND   (estabiliza la lectura)
 *                            └── R_INFERIOR ── GND
 *   Pila (-) ── GND común con el ESP32
 *
 * ⚠ FASE FUTURA: el circuito aún no está montado. Mientras
 * BATERIA_HABILITADA sea false, el módulo no toca ningún pin y las lecturas
 * devuelven NAN. Para activarlo, montar el divisor, ajustar los valores de
 * abajo al tipo de pila real y poner BATERIA_HABILITADA a true.
 *******************************************************************************/

#ifndef FUNCION_BATERIA_H_
#define FUNCION_BATERIA_H_

/************************* DEFINICIÓN DE CONSTANTES **************************/
#define BATERIA_HABILITADA false      // Cambiar a true cuando el circuito esté montado

#define BATERIA_PIN 35                // GPIO35 (ADC1, solo entrada)

// GPIO que conecta el divisor solo durante la medida, para no descargar la
// pila del comedero de forma continua. -1 = divisor siempre conectado.
#define BATERIA_PIN_ACTIVACION -1
#define BATERIA_TIEMPO_ESTABILIZACION_MS 30   // Espera tras activar el divisor

// Divisor resistivo (ohmios). Con 100k/100k se miden hasta ~6 V con un
// consumo de ~15 µA a 3 V.
#define BATERIA_R_SUPERIOR 100000.0
#define BATERIA_R_INFERIOR 100000.0

// Tensiones de referencia. Valores provisionales para 2 pilas AA alcalinas:
// AJUSTAR al tipo de pila que use el comedero.
#define BATERIA_V_LLENA 3.0
#define BATERIA_V_VACIA 2.2
#define BATERIA_AVISO_PORCENTAJE 20   // Por debajo de este % se genera aviso

#define BATERIA_NUM_MUESTRAS 16       // Lecturas promediadas por medida

/**
 * Configura los pines del módulo (no hace nada si está deshabilitado)
 */
void setupBateria();

/**
 * Mide la tensión de la pila
 * @return Tensión en voltios, o NAN si el módulo está deshabilitado
 */
float leerVoltajeBateria();

/**
 * Estima la carga restante a partir de la tensión
 * @param voltaje Tensión de la pila en voltios
 * @return Porcentaje 0-100, o -1 si la tensión no es válida
 */
int porcentajeBateria(float voltaje);

#endif // FUNCION_BATERIA_H_
