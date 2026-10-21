#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OLED.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void iniciarPantalla() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("No se pudo inicializar la pantalla OLED"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Pantalla OK");
  display.display();
  delay(1000);
}

void mostrarEnPantalla(float temperatura, float conductividad, bool nivelCorrecto) {
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("Temp: ");
  display.print(temperatura, 1);
  display.println(" C");

  display.print("TDS: ");
  display.print(conductividad, 0);
  display.println(" ppm");

  display.print("Nivel: ");
  display.println(nivelCorrecto ? "Alto" : "Bajo");

  display.display();
}