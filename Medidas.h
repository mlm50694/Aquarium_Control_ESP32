/*******************************************************************************
 * ESTRUCTURA COMÚN DE MEDIDAS
 *
 * Agrupa los últimos valores leídos para compartirlos entre módulos
 * (alarmas, pantalla y, en el futuro, envío por red).
 * Un valor NAN indica que la medida no está disponible.
 *******************************************************************************/

#ifndef MEDIDAS_H_
#define MEDIDAS_H_

struct Medidas {
  float temperatura;      // °C
  float conductividad;    // ppm (TDS compensado a 25 °C)
  bool nivelCorrecto;     // true: nivel correcto, false: nivel bajo
  float voltajeBateria;   // V, pila del comedero (NAN si deshabilitada)
};

#endif // MEDIDAS_H_
