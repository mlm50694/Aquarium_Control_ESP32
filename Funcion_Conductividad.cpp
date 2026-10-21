/*******************************************************************************
 * IMPLEMENTACIÓN DE FUNCIONES DEL SENSOR DE CONDUCTIVIDAD (TDS)
 *******************************************************************************/

#include <Arduino.h>
#include "Funcion_Conductividad.h"

// Buffer circular con las últimas muestras en milivoltios
static uint16_t muestras[TDS_NUM_MUESTRAS];
static uint8_t indiceMuestra = 0;
static uint8_t muestrasTomadas = 0;
static unsigned long ultimoMuestreo = 0;

void muestrearConductividad() {
    unsigned long ahora = millis();
    if (ahora - ultimoMuestreo < TDS_INTERVALO_MUESTREO_MS) {
        return;
    }
    ultimoMuestreo = ahora;

    // analogReadMilliVolts aplica la calibración de fábrica del ADC,
    // que en el ESP32 no es lineal si se usa analogRead directamente
    muestras[indiceMuestra] = analogReadMilliVolts(TDS_Sensor);
    indiceMuestra = (indiceMuestra + 1) % TDS_NUM_MUESTRAS;
    if (muestrasTomadas < TDS_NUM_MUESTRAS) {
        muestrasTomadas++;
    }
}

/**
 * Devuelve la mediana de las muestras disponibles
 */
static uint16_t medianaMuestras() {
    uint16_t ordenadas[TDS_NUM_MUESTRAS];
    for (uint8_t i = 0; i < muestrasTomadas; i++) {
        ordenadas[i] = muestras[i];
    }
    // Ordenación por inserción: suficiente para 30 elementos
    for (uint8_t i = 1; i < muestrasTomadas; i++) {
        uint16_t valor = ordenadas[i];
        int8_t j = i - 1;
        while (j >= 0 && ordenadas[j] > valor) {
            ordenadas[j + 1] = ordenadas[j];
            j--;
        }
        ordenadas[j + 1] = valor;
    }
    return ordenadas[muestrasTomadas / 2];
}

float leerConductividad(float temperatura) {
    if (muestrasTomadas == 0) {
        Serial.println("TDS: sin muestras todavía");
        return NAN;
    }

    float voltaje = medianaMuestras() / 1000.0;

    // Curva de calibración del fabricante (DFRobot): voltaje -> EC en µS/cm
    float ec = (133.42 * voltaje * voltaje * voltaje
              - 255.86 * voltaje * voltaje
              + 857.39 * voltaje) * TDS_FACTOR_K;

    // Compensación por temperatura: expresa la EC como si el agua estuviera a 25 °C.
    // Se divide (no se multiplica): el agua caliente conduce más y hay que restarlo.
    float tempCompensacion = isnan(temperatura) ? TDS_TEMPERATURA_REFERENCIA : temperatura;
    float ec25 = ec / (1.0 + TDS_COEF_TEMPERATURA * (tempCompensacion - TDS_TEMPERATURA_REFERENCIA));

    float tdsValue = ec25 * TDS_FACTOR_CONVERSION;
    if (tdsValue < 0) {
        tdsValue = 0;
    }

    // Mostrar el valor por el puerto serie
    Serial.print("TDS: ");
    Serial.print(tdsValue, 0);
    Serial.print(" ppm (");
    Serial.print(voltaje, 3);
    Serial.print(" V");
    if (isnan(temperatura)) {
        Serial.print(", sin compensar: falta temperatura");
    }
    Serial.println(")");
    return tdsValue;
}
