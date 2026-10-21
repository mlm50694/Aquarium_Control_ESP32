/*******************************************************************************
 * IMPLEMENTACIÓN DE FUNCIONES DEL SENSOR DE TEMPERATURA DS18B20
 *******************************************************************************/

#include <Arduino.h>
#include "Funcion_Temperatura.h"

// Variable para almacenar el valor de temperatura
float temperatura;

// Instanciación de objetos para el sensor DS18B20
OneWire oneWire(Temperature_Sensor);            // Objeto para protocolo OneWire
DallasTemperature sensors(&oneWire);            // Objeto para sensor DS18B20

/**
 * Inicializa el sensor de temperatura DS18B20
 */
void setupSensorTemperatura() {
    sensors.begin();  // Iniciar la biblioteca Dallas Temperature
}

/**
 * Lee la temperatura actual del sensor DS18B20
 * @return Temperatura en grados Celsius
 */
float leerTemperatura() {
    // Solicitar lectura a todos los sensores en el bus
    sensors.requestTemperatures();
    
    // Obtener temperatura del primer sensor (índice 0)
    // Si hubiera múltiples sensores, se podrían leer con índices diferentes
    temperatura = sensors.getTempCByIndex(0);
    
    // Mostrar el valor por el puerto serie
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println("°C");
    
    return temperatura;
}