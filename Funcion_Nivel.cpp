/*******************************************************************************
 * IMPLEMENTACIÓN DE FUNCIONES DEL SENSOR DE NIVEL DE AGUA
 *******************************************************************************/

#include <Arduino.h>
#include "Funcion_Nivel.h"

static bool estadoEstable = false;      // Último estado confirmado
static bool ultimaLectura = false;      // Última lectura cruda del pin
static unsigned long instanteCambio = 0;

/**
 * Lee el pin y lo traduce a "hay agua" según la lógica configurada
 */
static bool leerPinNivel() {
    bool alto = (digitalRead(LEVEL_SENSOR) == HIGH);
    return NIVEL_ACTIVO_ALTO ? alto : !alto;
}

void setupSensorNivel() {
    // Pull-down: si el cable se suelta, el pin lee BAJO y salta la alarma de
    // nivel en vez de dar lecturas aleatorias (fallo seguro)
    pinMode(LEVEL_SENSOR, INPUT_PULLDOWN);

    // La lectura inicial se acepta directamente para no esperar al arrancar
    ultimaLectura = leerPinNivel();
    estadoEstable = ultimaLectura;
    instanteCambio = millis();
}

void actualizarSensorNivel() {
    bool lectura = leerPinNivel();
    unsigned long ahora = millis();

    if (lectura != ultimaLectura) {
        // La lectura ha cambiado: se reinicia el tiempo de estabilidad
        ultimaLectura = lectura;
        instanteCambio = ahora;
    } else if (lectura != estadoEstable && ahora - instanteCambio >= NIVEL_TIEMPO_ESTABLE_MS) {
        estadoEstable = lectura;
    }
}

bool nivelCorrecto() {
    return estadoEstable;
}
