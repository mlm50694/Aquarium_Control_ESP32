/*******************************************************************************
 * IMPLEMENTACIÓN DE FUNCIONES DEL SENSOR DE CONDUCTIVIDAD (TDS)
 *******************************************************************************/

#include <Arduino.h>
#include "Funcion_Conductividad.h"

/**
 * Lee el valor del sensor de conductividad (TDS)
 * Realiza múltiples muestras para reducir ruido en la lectura
 * @return Valor de conductividad en ppm (partes por millón)
 */

float leerConductividad() {
    int valorADC = 0;          // Variable para acumular lecturas del ADC
    // Tomar múltiples muestras para reducir el ruido
    for (int i = 0; i < SAMPLES; i++) {
        valorADC += analogRead(TDS_Sensor);  // Acumular lecturas
        delay(10);             // Pequeña pausa entre lecturas
    }
    
    // Calcular el promedio de las lecturas
    valorADC = valorADC / SAMPLES;
    
    // Convertir lectura ADC a voltaje (ADC ESP32 es de 12 bits = 4095)
    float voltage = valorADC * (VREF / 4095.0);
    
    // Convertir voltaje a valor de conductividad en ppm
    // Esta es una fórmula simplificada. Para mayor precisión se
    // podría implementar compensación por temperatura
    float tdsValue = (voltage / VREF) * 1000;
    
    // Mostrar el valor por el puerto serie
    Serial.print("TDS: ");
    Serial.print(tdsValue);
    Serial.println(" ppm");
    return tdsValue;  // Devuelve el valor de conductividad
}