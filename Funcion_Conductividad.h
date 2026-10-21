/*******************************************************************************
 * MÓDULO DE SENSOR DE CONDUCTIVIDAD (TDS)
 * 
 * Este archivo define las constantes y funciones para el sensor de conductividad.
 *******************************************************************************/

#ifndef FUNCION_CONDUCTIVIDAD_H_
#define FUNCION_CONDUCTIVIDAD_H_

/************************* DEFINICIÓN DE CONSTANTES **************************/

// Pin del sensor de conductividad TDS
#define TDS_Sensor 34         // GPIO34 conectado al sensor de conductividad

// Parámetros para lectura y cálculo
#define VREF 3.3              // Voltaje de referencia del ADC (3.3V en ESP32)
#define SAMPLES 10            // Número de muestras para promedio y reducción de ruido

/**
 * Obtiene la lectura del sensor de conductividad
 * @return Valor de conductividad en ppm (partes por millón)
 */
float leerConductividad();

#endif // FUNCION_CONDUCTIVIDAD_H_