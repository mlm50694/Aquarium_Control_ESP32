/*******************************************************************************
 * IMPLEMENTACIÓN DE FUNCIONES DE SINCRONIZACIÓN NTP
 *******************************************************************************/

#include <Arduino.h>
#include "time.h"
#include "NTP.h"

// Definición de constantes
const char* ntpServer = "pool.ntp.org";  // Servidor NTP predeterminado

/**
 * Configura y sincroniza la hora mediante NTP
 * Utiliza la zona horaria de España (Central European Time)
 */
void setupNTP() {
  // Configuración NTP con zona horaria de España
  configTzTime("CET-1CEST,M3.5.0/2,M10.5.0/3", ntpServer);
  
  // Esperamos a que se sincronice la hora
  struct tm timeinfo;
  int intentos = 0;
  const int maxIntentos = 10;  // Número máximo de intentos para evitar bloqueos
  
  Serial.println("Iniciando sincronización NTP...");
  
  while (!getLocalTime(&timeinfo) && intentos < maxIntentos) {
    Serial.println("Esperando sincronización NTP...");
    intentos++;
    delay(1000);
  }
  
  if (intentos < maxIntentos) {
    Serial.println("Hora obtenida:");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  } else {
    Serial.println("No se pudo sincronizar con el servidor NTP");
  }
}

/**
 * Imprime la hora local actual por el puerto serie
 */
void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Error al obtener la hora");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}