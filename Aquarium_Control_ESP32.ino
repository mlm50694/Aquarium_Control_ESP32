/*******************************************************************************
 * SISTEMA DE CONTROL IOT PARA ACUARIO CON ESP32
 * 
 * Este programa implementa un sistema para monitorizar parámetros cruciales
 * de un acuario: temperatura, conductividad (TDS) y nivel de agua, además de la batería
 * del comedero automático.
 * 
 * Autor: Miguel Ángel López Moreno
 * Fecha: 2025
 *******************************************************************************/

/********************************* LIBRERÍAS ***********************************/
#include "Arduino.h"
#include "WiFi.h"                // Librería para la conexión WiFi
#include "time.h"
#include "config.h"              // Archivo con credenciales WiFi (ssid y password)
#include "Funcion_Temperatura.h" // Funciones para el sensor de temperatura
#include "Funcion_Conductividad.h" // Funciones para el sensor de conductividad
#include "NTP.h"
#include "Atenuacion_Luz.h"
#include "OLED.h"

/************************* DEFINICIÓN DE CONSTANTES **************************/
// Pin del sensor de nivel de agua
#define LEVEL_SENSOR 25          // GPIO25 conectado al sensor de nivel de agua

// Umbrales para alarmas de parámetros
#define AVISO_CONDUCTIVIDAD_MEDIA 200  // Valor en ppm para alerta de conductividad media
#define ALARMA_CONDUCTIVIDAD_ALTA 300  // Valor en ppm para alarma de conductividad alta
#define ALARMA_TEMPERATURA_BAJA 20     // Valor en °C para alarma de temperatura baja
#define ALARMA_TEMPERATURA_ALTA 30     // Valor en °C para alarma de temperatura alta

/**************************** FUNCIONES AUXILIARES ****************************/

/**
 * Inicializa la conexión WiFi en modo estación
 * Muestra el progreso de la conexión y la IP asignada
 */
void initWiFi() {
  WiFi.mode(WIFI_STA);           // ESP32 actúa en modo estación para conectarse a un punto de acceso
  
  //Configuración de IP estática y DNS
  IPAddress localIP(192, 168, 1, 143);        // IP estática
  IPAddress gateway(192, 168, 1, 1);          // Puerta de enlace (router)
  IPAddress subnet(255, 255, 255, 0);         // Máscara de subred
  IPAddress dns1(8, 8, 8, 8);                 // DNS primario (Google)
  IPAddress dns2(8, 8, 4, 4);                 // DNS secundario (Google)

  if (!WiFi.config(localIP, gateway, subnet, dns1, dns2)) {
    Serial.println("Error en la configuración de IP estática");
  }
  
  WiFi.begin(ssid, password);

  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  
  Serial.println();
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
}

/**
 * Verifica los valores de los sensores y genera alertas si es necesario
 * @param temperatura Valor de temperatura en °C
 * @param conductividad Valor de conductividad en ppm
 * @param nivel Estado del nivel de agua (true: correcto, false: bajo)
 */
void verificarAlarmas(float temperatura, float conductividad, bool nivel) {
  // Verificar temperatura
  if (temperatura < ALARMA_TEMPERATURA_BAJA) {
    Serial.println("¡ALARMA! Temperatura demasiado baja");
  } else if (temperatura > ALARMA_TEMPERATURA_ALTA) {
    Serial.println("¡ALARMA! Temperatura demasiado alta");
  }
  
  // Verificar conductividad
  if (conductividad > ALARMA_CONDUCTIVIDAD_ALTA) {
    Serial.println("¡ALARMA! Conductividad demasiado alta");
  } else if (conductividad > AVISO_CONDUCTIVIDAD_MEDIA) {
    Serial.println("AVISO: Conductividad media-alta");
  }
  
  // Verificar nivel de agua
  if (nivel == 0) {
    Serial.println("¡ALARMA! Nivel de agua bajo");
  }
}

/****************************** SETUP Y LOOP *********************************/

void setup() {
  Serial.begin(115200);          // Inicializar comunicación serial
  Serial.println("\n--- Iniciando sistema de monitoreo de acuario ---");
  iniciarPantalla();

  pinMode(LEVEL_SENSOR, INPUT);  // Configurar pin del sensor de nivel como entrada
  pinMode(LED_PIN, OUTPUT);

  // Inicializar conexión WiFi
  initWiFi();
  Serial.print("RSSI: ");        // RSSI es la intensidad de la señal WiFi
  Serial.println(WiFi.RSSI());
  
  // Inicializar sincronización de hora
  setupNTP();
  
  // Inicializar sensores
  setupSensorTemperatura();      // Inicializar sensor de temperatura DS18B20
  
  // Esperar a que el sistema se estabilice
  delay(1000);
  
  Serial.println("Sistema de monitoreo de acuario iniciado");
}

unsigned long tiempoUltimaLectura = 0;
const unsigned long intervaloLectura = 30000;  // 30 segundos

void loop() {
  
  unsigned long ahora = millis();
  if (ahora - tiempoUltimaLectura >= intervaloLectura) {
    tiempoUltimaLectura = ahora;
  // 1. Mostrar la hora actual
  printLocalTime();
  
  // 2. Leer sensor de temperatura
  float temperatura = leerTemperatura();  // Lectura en °C
  
  // 3. Leer sensor de conductividad
  float conductividad = leerConductividad();  // Lectura en ppm
  
  // 4. Leer sensor de nivel
  bool nivelCorrecto = digitalRead(LEVEL_SENSOR);
  
  // 5. Verificar condiciones de alarma
  verificarAlarmas(temperatura, conductividad, nivelCorrecto);

  mostrarEnPantalla(temperatura, conductividad, nivelCorrecto);

  // Línea separadora para mejorar legibilidad
  Serial.println("----------------------------------------");
  }

  // 6. Atenuación de Luz
  actualizarAtenuacion();
 
  // Pequeña pausa para evitar sobrecarga
  delay(100);
}