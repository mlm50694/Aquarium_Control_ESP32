#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OLED.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C
#define OLED_ALTO_FRANJA_AMARILLA 16   // Las 16 filas superiores son amarillas

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

static bool pantallaDisponible = false;

bool iniciarPantalla() {
  // display.begin() solo falla si no puede reservar memoria: no comprueba que
  // la pantalla esté conectada. Por eso se pregunta además por el bus I2C.
  bool memoriaOk = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  Wire.beginTransmission(OLED_ADDRESS);
  bool responde = (Wire.endTransmission() == 0);

  if (!memoriaOk || !responde) {
    // Antes se hacía while(true): un fallo de la pantalla dejaba sin vigilancia
    // toda la pecera. Ahora se sigue funcionando sin pantalla.
    Serial.println(F("No se pudo inicializar la pantalla OLED, se continúa sin ella"));
    pantallaDisponible = false;
    return false;
  }

  pantallaDisponible = true;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Pantalla OK");
  display.display();
  return true;
}

/**
 * Escribe un valor con decimales, o "--" si no está disponible
 */
static void imprimirValor(float valor, int decimales) {
  if (isnan(valor)) {
    display.print("--");
  } else {
    display.print(valor, decimales);
  }
}

void mostrarEnPantalla(const Medidas& medidas, float brilloLuz) {
  if (!pantallaDisponible) {
    return;
  }

  display.clearDisplay();

  // Franja amarilla: hora y brillo de la luz
  display.setCursor(0, 4);
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 0)) {
    display.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  } else {
    display.print("--:--");
  }
  display.setCursor(72, 4);
  display.printf("Luz %3d%%", (int)(brilloLuz * 100 + 0.5));

  // Zona azul: medidas
  display.setCursor(0, OLED_ALTO_FRANJA_AMARILLA + 2);
  display.print("Temp:  ");
  imprimirValor(medidas.temperatura, 1);
  display.println(" C");

  display.setCursor(0, OLED_ALTO_FRANJA_AMARILLA + 14);
  display.print("TDS:   ");
  imprimirValor(medidas.conductividad, 0);
  display.println(" ppm");

  display.setCursor(0, OLED_ALTO_FRANJA_AMARILLA + 26);
  display.print("Nivel: ");
  display.println(medidas.nivelCorrecto ? "Correcto" : "BAJO");

  // La línea de la pila solo aparece cuando el módulo está habilitado
  if (!isnan(medidas.voltajeBateria)) {
    display.setCursor(0, OLED_ALTO_FRANJA_AMARILLA + 38);
    display.print("Pila:  ");
    imprimirValor(medidas.voltajeBateria, 2);
    display.println(" V");
  }

  display.display();
}
