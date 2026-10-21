#ifndef PANTALLAOLED_H
#define PANTALLAOLED_H

#include <Arduino.h>
#include "Medidas.h"

/**
 * Inicializa la pantalla. Si no responde, el sistema sigue funcionando sin ella.
 * @return true si la pantalla está disponible
 */
bool iniciarPantalla();

/**
 * Dibuja la hora y el brillo de la luz en la franja amarilla superior y las
 * medidas en la zona azul. No hace nada si la pantalla no está disponible.
 * @param medidas Últimas medidas leídas
 * @param brilloLuz Brillo actual de la lámpara (0.0-1.0)
 */
void mostrarEnPantalla(const Medidas& medidas, float brilloLuz);

#endif
