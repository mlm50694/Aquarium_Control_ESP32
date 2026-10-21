/*******************************************************************************
 * IMPLEMENTACIÓN DE FUNCIONES DE SINCRONIZACIÓN NTP
 *******************************************************************************/

#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "NTP.h"

// Definición de constantes
const char* ntpServer = "pool.ntp.org";  // Servidor NTP predeterminado

/**
 * Configura y sincroniza la hora mediante NTP
 * Utiliza la zona horaria de España (Central European Time)
 */
void setupNTP() {
  // Configuración NTP con zona horaria de España. El cliente SNTP sigue
  // funcionando en segundo plano: si ahora no hay red, sincronizará en cuanto
  // vuelva la conexión, sin necesidad de llamar otra vez a esta función.
  configTzTime("CET-1CEST,M3.5.0/2,M10.5.0/3", ntpServer);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("NTP: sin WiFi, la hora se sincronizará al conectar");
    return;
  }

  Serial.println("Iniciando sincronización NTP...");

  // Una sola espera de hasta 10 s. Antes se llamaba a getLocalTime() en un
  // bucle de 10 intentos, y cada llamada ya espera 5 s por defecto (hasta 60 s)
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, NTP_TIMEOUT_MS)) {
    Serial.println("Hora obtenida:");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  } else {
    Serial.println("No se pudo sincronizar con el servidor NTP, se reintentará en segundo plano");
  }
}

/**
 * Imprime la hora local actual por el puerto serie
 */
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 0)) {   // 0 ms: no esperar si aún no hay hora
    Serial.println("Hora no disponible (NTP sin sincronizar)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
