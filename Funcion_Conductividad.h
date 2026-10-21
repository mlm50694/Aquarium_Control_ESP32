/*******************************************************************************
 * MÓDULO DE SENSOR DE CONDUCTIVIDAD (TDS)
 *
 * Este archivo define las constantes y funciones para el sensor de conductividad.
 * Sensor: TDS Meter V1.0 (compatible DFRobot Gravity SEN0244)
 *   - Alimentación 3,3-5,5 V, salida analógica 0-2,3 V, rango 0-1000 ppm
 *******************************************************************************/

#ifndef FUNCION_CONDUCTIVIDAD_H_
#define FUNCION_CONDUCTIVIDAD_H_

/************************* DEFINICIÓN DE CONSTANTES **************************/

// Pin del sensor de conductividad TDS
#define TDS_Sensor 34         // GPIO34 (ADC1, compatible con WiFi activo)

// Muestreo: se toma una muestra cada 40 ms en segundo plano y se usa la
// mediana de las últimas 30 (descarta picos de ruido mejor que la media)
#define TDS_NUM_MUESTRAS 30
#define TDS_INTERVALO_MUESTREO_MS 40

// Parámetros de cálculo
#define TDS_TEMPERATURA_REFERENCIA 25.0  // °C a la que se expresa el resultado
#define TDS_COEF_TEMPERATURA 0.02        // Variación de la conductividad: 2 % por °C
#define TDS_FACTOR_CONVERSION 0.5        // EC (µS/cm) -> TDS (ppm), agua dulce
#define TDS_FACTOR_K 1.0                 // Factor de calibración (ver README)

/**
 * Toma una muestra del ADC si ha pasado el intervalo de muestreo.
 * Debe llamarse en cada vuelta de loop(); no bloquea.
 */
void muestrearConductividad();

/**
 * Calcula el TDS a partir de la mediana de las muestras tomadas
 * @param temperatura Temperatura del agua en °C para la compensación
 *                    (NAN si no está disponible: se calcula sin compensar)
 * @return Valor de TDS en ppm compensado a 25 °C, o NAN si aún no hay muestras
 */
float leerConductividad(float temperatura);

#endif // FUNCION_CONDUCTIVIDAD_H_
