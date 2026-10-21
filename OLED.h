#ifndef PANTALLAOLED_H
#define PANTALLAOLED_H

#include <Arduino.h>

void iniciarPantalla();
void mostrarEnPantalla(float temperatura, float conductividad, bool nivelCorrecto);

#endif