/*******************************************************************************
 * MÓDULO DE SENSOR DE TEMPERATURA DS18B20
 * 
 * Este archivo define las constantes y funciones para el sensor de temperatura.
 *******************************************************************************/

#ifndef FUNCION_TEMPERATURA_H_
#define FUNCION_TEMPERATURA_H_

/************************** INCLUSIÓN DE LIBRERÍAS ****************************/
#include <OneWire.h>          // Comunicación con dispositivos OneWire
#include <DallasTemperature.h> // Interfaz específica para sensores DS18B20

/************************* DEFINICIÓN DE CONSTANTES **************************/
#define Temperature_Sensor 4   // GPIO4 conectado al sensor DS18B20

// Declaraciones de objetos compartidos
extern OneWire oneWire;           // Objeto para comunicación OneWire
extern DallasTemperature sensors; // Objeto para gestión de sensores DS18B20

/**
 * Inicializa el sensor de temperatura
 */
void setupSensorTemperatura();

/**
 * Lee la temperatura del sensor DS18B20
 * @return Temperatura en grados Celsius
 */
float leerTemperatura();

#endif // FUNCION_TEMPERATURA_H_