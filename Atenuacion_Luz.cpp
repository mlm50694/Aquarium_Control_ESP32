#include <Arduino.h>
#include "Atenuacion_Luz.h"

enum EstadoLuz { AMANECER, DIA_COMPLETO, ANOCHECER, NOCHE };

static EstadoLuz estadoActual = NOCHE;
static int brillo = 0;
static bool primeraEjecucion = true;

void iniciarAtenuacion() {
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0); // Comenzar con la luz apagada
}

void actualizarAtenuacion() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Error al obtener la hora para control de luz");
    return;
  }

  int horaActual = timeinfo.tm_hour;
  int minutoActual = timeinfo.tm_min;

  // Determinar el estado actual basado en la hora
  if (horaActual >= AMANECER_HORA && horaActual < DIA_COMPLETO_HORA) {
    estadoActual = AMANECER;
  } else if (horaActual >= DIA_COMPLETO_HORA && horaActual < ANOCHECER_HORA) {
    estadoActual = DIA_COMPLETO;
  } else if (horaActual >= ANOCHECER_HORA && horaActual < NOCHE_HORA) {
    estadoActual = ANOCHECER;
  } else {
    estadoActual = NOCHE;
  }

  // Calcular el progreso de la atenuación basado en el tiempo actual
  int minutosTranscurridos;
  float progreso;

  switch (estadoActual) {
    case AMANECER:
      minutosTranscurridos = (horaActual - AMANECER_HORA) * 60 + minutoActual;
      progreso = (float)minutosTranscurridos / 60.0; // 60 minutos = 1 hora
      brillo = (int)(MAX_BRIGHTNESS * progreso);
      break;

    case DIA_COMPLETO:
      brillo = MAX_BRIGHTNESS;
      break;

    case ANOCHECER:
      minutosTranscurridos = (horaActual - ANOCHECER_HORA) * 60 + minutoActual;
      progreso = 1.0 - ((float)minutosTranscurridos / 60.0); // 60 minutos = 1 hora
      brillo = (int)(MAX_BRIGHTNESS * progreso);
      break;

    case NOCHE:
      brillo = 0;
      break;
  }

  // Asegurarse de que el brillo está dentro de los límites
  brillo = constrain(brillo, 0, MAX_BRIGHTNESS);
  
  // Aplicar el brillo al LED
  analogWrite(LED_PIN, brillo);
}