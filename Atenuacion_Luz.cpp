#include <Arduino.h>
#include <esp_arduino_version.h>
#include "Atenuacion_Luz.h"

#define SEGUNDOS_DIA 86400L

enum EstadoLuz { AMANECER, DIA_COMPLETO, ANOCHECER, NOCHE };
static const char* NOMBRES_ESTADO[] = { "AMANECER", "DIA_COMPLETO", "ANOCHECER", "NOCHE" };

static HorarioLuz horario = {
  AMANECER_HORA, AMANECER_MINUTO,
  ANOCHECER_HORA, ANOCHECER_MINUTO,
  RAMP_TIME, BRILLO_MAXIMO_PCT
};

static float brillo = 0.0;                 // Brillo percibido actual (0.0-1.0)
static uint32_t dutyAplicado = 0;
static EstadoLuz estadoAnterior = NOCHE;
static bool hayHora = false;
static unsigned long ultimaActualizacion = 0;

static void escribirPWM(uint32_t duty) {
  // La API de LEDC cambió en el core 3.x: se direcciona por pin y no por canal
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(LED_PIN, duty);
#else
  ledcWrite(LUZ_PWM_CANAL, duty);
#endif
}

void iniciarAtenuacion() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LED_PIN, LUZ_PWM_FRECUENCIA, LUZ_PWM_RESOLUCION);
#else
  ledcSetup(LUZ_PWM_CANAL, LUZ_PWM_FRECUENCIA, LUZ_PWM_RESOLUCION);
  ledcAttachPin(LED_PIN, LUZ_PWM_CANAL);
#endif
  escribirPWM(0); // Comenzar con la luz apagada
}

/**
 * Segundos que hay desde "desde" hasta "hasta", dando la vuelta a medianoche
 */
static long segundosEntre(long desde, long hasta) {
  return ((hasta - desde) % SEGUNDOS_DIA + SEGUNDOS_DIA) % SEGUNDOS_DIA;
}

/**
 * Determina la fase del día. Usa la misma lógica que calcularBrilloObjetivo.
 */
static EstadoLuz calcularEstado(long segundosDia, const HorarioLuz& h) {
  long inicioAmanecer = h.amanecerHora * 3600L + h.amanecerMinuto * 60L;
  long inicioAnochecer = h.anochecerHora * 3600L + h.anochecerMinuto * 60L;
  long t = segundosEntre(inicioAmanecer, segundosDia);           // Desde el amanecer
  long duracionDia = segundosEntre(inicioAmanecer, inicioAnochecer);

  if (t < h.duracionRampa_s) return AMANECER;
  if (t < duracionDia) return DIA_COMPLETO;
  if (t < duracionDia + h.duracionRampa_s) return ANOCHECER;
  return NOCHE;
}

float calcularBrilloObjetivo(long segundosDia, const HorarioLuz& h) {
  long inicioAmanecer = h.amanecerHora * 3600L + h.amanecerMinuto * 60L;
  long inicioAnochecer = h.anochecerHora * 3600L + h.anochecerMinuto * 60L;
  long t = segundosEntre(inicioAmanecer, segundosDia);
  long duracionDia = segundosEntre(inicioAmanecer, inicioAnochecer);
  float rampa = h.duracionRampa_s;
  float nivel;

  switch (calcularEstado(segundosDia, h)) {
    case AMANECER:     nivel = t / rampa; break;
    case DIA_COMPLETO: nivel = 1.0; break;
    case ANOCHECER:    nivel = 1.0 - (t - duracionDia) / rampa; break;
    default:           nivel = 0.0; break;
  }
  return constrain(nivel, 0.0f, 1.0f) * h.brilloMaximo_pct / 100.0;
}

void actualizarAtenuacion() {
  unsigned long ahora = millis();
  if (ahora - ultimaActualizacion < LUZ_INTERVALO_ACTUALIZACION_MS) {
    return;
  }
  ultimaActualizacion = ahora;

  // Timeout 0: getLocalTime() por defecto espera hasta 5 s a tener hora
  // válida, lo que bloquearía todo el loop() si el NTP no ha sincronizado
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 0)) {
    if (hayHora) {
      Serial.println("Luz: sin hora válida, se mantiene el brillo actual");
      hayHora = false;
    }
    return;
  }
  hayHora = true;

  long segundosDia = timeinfo.tm_hour * 3600L + timeinfo.tm_min * 60L + timeinfo.tm_sec;
  brillo = calcularBrilloObjetivo(segundosDia, horario);

  // Corrección gamma: el ojo no percibe el brillo de forma lineal. Sin ella,
  // la primera mitad de la rampa parece casi a pleno brillo.
  uint32_t duty = (uint32_t)(powf(brillo, LUZ_GAMMA) * LUZ_PWM_MAX + 0.5f);
  if (duty != dutyAplicado) {
    escribirPWM(duty);
    dutyAplicado = duty;
  }

  EstadoLuz estado = calcularEstado(segundosDia, horario);
  if (estado != estadoAnterior) {
    Serial.printf("Luz: %02d:%02d -> %s (brillo %d %%)\n",
                  timeinfo.tm_hour, timeinfo.tm_min, NOMBRES_ESTADO[estado], (int)(brillo * 100));
    estadoAnterior = estado;
  }
}

bool configurarHorarioLuz(const HorarioLuz& nuevoHorario) {
  if (nuevoHorario.amanecerHora > 23 || nuevoHorario.anochecerHora > 23 ||
      nuevoHorario.amanecerMinuto > 59 || nuevoHorario.anochecerMinuto > 59 ||
      nuevoHorario.brilloMaximo_pct > 100 || nuevoHorario.duracionRampa_s == 0) {
    return false;
  }

  // Las dos rampas deben caber: la de amanecer antes del anochecer y la de
  // anochecer antes del amanecer siguiente
  long inicioAmanecer = nuevoHorario.amanecerHora * 3600L + nuevoHorario.amanecerMinuto * 60L;
  long inicioAnochecer = nuevoHorario.anochecerHora * 3600L + nuevoHorario.anochecerMinuto * 60L;
  long duracionDia = segundosEntre(inicioAmanecer, inicioAnochecer);
  long duracionNoche = SEGUNDOS_DIA - duracionDia;
  if (nuevoHorario.duracionRampa_s > duracionDia || nuevoHorario.duracionRampa_s > duracionNoche) {
    return false;
  }

  horario = nuevoHorario;
  ultimaActualizacion = millis() - LUZ_INTERVALO_ACTUALIZACION_MS;  // Aplicar ya
  return true;
}

HorarioLuz horarioLuzActual() {
  return horario;
}

float brilloLuzActual() {
  return brillo;
}
