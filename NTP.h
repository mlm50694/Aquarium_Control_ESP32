/*******************************************************************************
 * MÓDULO DE SINCRONIZACIÓN NTP (NETWORK TIME PROTOCOL)
 * 
 * Este archivo define las constantes y funciones para la sincronización 
 * de hora mediante servidores NTP.
 *******************************************************************************/

#ifndef NTP_H_
#define NTP_H_

#include <Arduino.h>
#include "time.h"

// Declaración de constantes
extern const char* ntpServer;  // Servidor NTP a utilizar

#define NTP_TIMEOUT_MS 10000   // Espera máxima a la primera sincronización

// Declaración de funciones
void setupNTP();               // Configura y sincroniza la hora mediante NTP
void printLocalTime();         // Imprime la hora local actual

#endif // NTP_H_