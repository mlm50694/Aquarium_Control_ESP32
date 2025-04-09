// Funcion_Temperatura.h
#ifndef FUNCION_TEMPERATURA_H
#define FUNCION_TEMPERATURA_H

////////// CONFIGURACION DEL SENSOR DE TEMPERATURA DS18B20 //////////
#include <OneWire.h> // Librería para la comunicación con dispositivos OneWire (como el sensor DS18B20)
#include <DallasTemperature.h> // Librería específica para manejar sensores de temperatura DS18B20 con OneWire

#define Temperature_Sensor 4  // Pin al que está conectado el DS18B20

// Declaraciones (solo anuncio, sin inicializar)e
extern OneWire Wire;
extern DallasTemperature sensors;

void setupSensorTemperatura();  // Declarar función para inicializar el sensor
float leerTemperatura();  // Declarar función para leer la temperatura

#endif