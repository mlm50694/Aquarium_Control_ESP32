/*******************************************************************************
 * IMPLEMENTACIÓN DE LA MEDIDA DE LA PILA DEL COMEDERO AUTOMÁTICO
 *******************************************************************************/

#include <Arduino.h>
#include "Funcion_Bateria.h"

void setupBateria() {
    if (!BATERIA_HABILITADA) {
        return;
    }
    if (BATERIA_PIN_ACTIVACION >= 0) {
        pinMode(BATERIA_PIN_ACTIVACION, OUTPUT);
        digitalWrite(BATERIA_PIN_ACTIVACION, LOW);   // Divisor desconectado en reposo
    }
}

float leerVoltajeBateria() {
    if (!BATERIA_HABILITADA) {
        return NAN;
    }

    if (BATERIA_PIN_ACTIVACION >= 0) {
        digitalWrite(BATERIA_PIN_ACTIVACION, HIGH);
        delay(BATERIA_TIEMPO_ESTABILIZACION_MS);     // Carga del condensador del divisor
    }

    uint32_t sumaMilivoltios = 0;
    for (int i = 0; i < BATERIA_NUM_MUESTRAS; i++) {
        sumaMilivoltios += analogReadMilliVolts(BATERIA_PIN);
    }

    if (BATERIA_PIN_ACTIVACION >= 0) {
        digitalWrite(BATERIA_PIN_ACTIVACION, LOW);
    }

    // Deshacer el divisor para obtener la tensión real de la pila
    float voltajePin = (sumaMilivoltios / (float)BATERIA_NUM_MUESTRAS) / 1000.0;
    float voltaje = voltajePin * (BATERIA_R_SUPERIOR + BATERIA_R_INFERIOR) / BATERIA_R_INFERIOR;

    Serial.print("Pila comedero: ");
    Serial.print(voltaje, 2);
    Serial.print(" V (");
    Serial.print(porcentajeBateria(voltaje));
    Serial.println(" %)");
    return voltaje;
}

int porcentajeBateria(float voltaje) {
    if (isnan(voltaje)) {
        return -1;
    }
    // Aproximación lineal: suficiente para avisar del cambio de pila
    float porcentaje = (voltaje - BATERIA_V_VACIA) / (BATERIA_V_LLENA - BATERIA_V_VACIA) * 100.0;
    return constrain((int)(porcentaje + 0.5), 0, 100);
}
