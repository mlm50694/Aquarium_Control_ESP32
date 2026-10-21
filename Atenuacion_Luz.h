/*******************************************************************************
 * ATENUACION DE LUZ
 *
 * Esta función realiza los modos amanecer/atardecer en las hora determinadas
 * del día.
 *
 * El brillo se calcula a partir de la hora actual (no de un contador), así
 * que tras un reinicio la luz vuelve directamente al nivel que le corresponde.
 *
 * Mejoras respecto a la versión con analogWrite:
 *   - PWM de 13 bits (8192 niveles) en lugar de 8 bits (256)
 *   - Resolución de segundos en lugar de minutos: rampa continua, sin saltos
 *   - Corrección gamma: el ojo percibe la rampa uniforme también a bajo brillo
 *   - Horario modificable en tiempo de ejecución (para la futura app web)
 *******************************************************************************/

#ifndef ATENUACION_LUZ_H_
#define ATENUACION_LUZ_H_

#include <Arduino.h>

/************************* DEFINICIÓN DE CONSTANTES **************************/
// ⚠ GPIO15 es un pin de arranque y emite PWM durante el boot: la lámpara puede
// parpadear al reiniciar. Pendiente de hardware: mover a GPIO26 (ver README).
#define LED_PIN 15

#define LUZ_PWM_FRECUENCIA 5000       // Hz (con 13 bits el máximo es ~9,7 kHz)
#define LUZ_PWM_RESOLUCION 13         // bits
#define LUZ_PWM_MAX ((1 << LUZ_PWM_RESOLUCION) - 1)
#define LUZ_PWM_CANAL 0               // Solo se usa con el core ESP32 2.x
#define LUZ_GAMMA 2.2                 // Curva de percepción del ojo humano
#define LUZ_INTERVALO_ACTUALIZACION_MS 200

// Horario por defecto (formato 24 horas)
#define AMANECER_HORA 13              // Empieza el amanecer a las 13:00
#define AMANECER_MINUTO 0
#define ANOCHECER_HORA 20             // Empieza el anochecer a las 20:00
#define ANOCHECER_MINUTO 0
#define RAMP_TIME 3600                // Duración de cada rampa (en segundos)
#define BRILLO_MAXIMO_PCT 100         // Brillo a pleno día (%)

/**
 * Horario de la luz. Los campos de inicio marcan cuándo EMPIEZA cada rampa:
 * con los valores por defecto la luz sube de 13:00 a 14:00 y baja de 20:00 a 21:00.
 */
struct HorarioLuz {
  uint8_t amanecerHora;
  uint8_t amanecerMinuto;
  uint8_t anochecerHora;
  uint8_t anochecerMinuto;
  uint16_t duracionRampa_s;
  uint8_t brilloMaximo_pct;
};

/**
 * Configura el PWM y deja la luz apagada
 */
void iniciarAtenuacion();

/**
 * Recalcula y aplica el brillo según la hora. Llamar en cada vuelta de loop();
 * no bloquea. Sin hora NTP válida mantiene el último brillo aplicado.
 */
void actualizarAtenuacion();

/**
 * Cambia el horario de la luz (pensado para la futura app web)
 * @return false si el horario no es válido (no se aplica)
 */
bool configurarHorarioLuz(const HorarioLuz& nuevoHorario);

/**
 * @return Horario actualmente en uso
 */
HorarioLuz horarioLuzActual();

/**
 * @return Brillo actual percibido, de 0.0 (apagado) a 1.0 (máximo)
 */
float brilloLuzActual();

/**
 * Brillo que corresponde a un instante del día según un horario
 * @param segundosDia Segundos desde las 00:00 (0-86399)
 * @return Brillo percibido de 0.0 a 1.0 (antes de la corrección gamma)
 */
float calcularBrilloObjetivo(long segundosDia, const HorarioLuz& horario);

#endif
