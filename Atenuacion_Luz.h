/*******************************************************************************
 * ATENUACION DE LUZ
 * 
 * Esta función realiza los modos amanecer/atardecer en las hora determinadas
 * del día.
 *******************************************************************************/

#ifndef ATENUACION_LUZ_H_
#define ATENUACION_LUZ_H_

#define LED_PIN 15
#define RAMP_TIME 3600  // Tiempo total de atenuación (en segundos)
#define MAX_BRIGHTNESS 255

// Horarios de control (formato de 24 horas)
#define AMANECER_HORA 13  // 13:00 (1 PM)
#define DIA_COMPLETO_HORA 14  // 14:00 (2 PM)
#define ANOCHECER_HORA 20  // 20:00 (8 PM)
#define NOCHE_HORA 21  // 21:00 (9 PM)

void iniciarAtenuacion();
void actualizarAtenuacion();
bool comprobarHorarioLuz(int horaActual, int minutoActual);

#endif