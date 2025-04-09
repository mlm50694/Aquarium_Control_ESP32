// Funcion_Temperatura.cpp
#include "Funcion_Temperatura.h"

// Definir el objeto OneWire y el objeto DallasTemperature
OneWire Wire(Temperature_Sensor);
DallasTemperature sensors(&Wire);

void setupSensorTemperatura() {
    sensors.begin();  // Inicializa el sensor de temperatura
}

float leerTemperatura() {
    sensors.requestTemperatures();  // Solicita la lectura de temperatura
    return sensors.getTempCByIndex(0);  // Devuelve la temperatura en grados Celsius
}