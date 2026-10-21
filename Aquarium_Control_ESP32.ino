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
#include "Medidas.h"             // Estructura común con las últimas medidas
#include "Funcion_Temperatura.h" // Funciones para el sensor de temperatura
#include "Funcion_Conductividad.h" // Funciones para el sensor de conductividad
#include "Funcion_Nivel.h"       // Funciones para el sensor de nivel
#include "Funcion_Bateria.h"     // Medida de la pila del comedero (fase futura)
#include "NTP.h"
#include "Atenuacion_Luz.h"
#include "OLED.h"

/************************* DEFINICIÓN DE CONSTANTES **************************/
// Umbrales para alarmas de parámetros (según el documento técnico)
#define AVISO_CONDUCTIVIDAD_MEDIA 150  // ppm: límite superior del rango ideal (50-150 ppm)
#define ALARMA_CONDUCTIVIDAD_ALTA 300  // ppm
#define ALARMA_TEMPERATURA_BAJA 20     // °C: nunca por debajo
#define AVISO_TEMPERATURA_BAJA 22      // °C: rango ideal 22-26 °C
#define AVISO_TEMPERATURA_ALTA 26      // °C
#define ALARMA_TEMPERATURA_ALTA 28     // °C: nunca por encima

// Intervalos de las tareas periódicas
#define INTERVALO_LECTURA_MS 30000     // Lectura de sensores: 30 segundos
#define INTERVALO_PANTALLA_MS 1000     // Refresco de la pantalla: 1 segundo
#define INTERVALO_BATERIA_MS 600000    // Medida de la pila: 10 minutos
#define RETARDO_PRIMERA_LECTURA_MS 2000 // Tiempo para llenar el buffer del TDS

// Conexión WiFi
#define WIFI_TIMEOUT_CONEXION_MS 20000     // Espera máxima al arrancar
#define WIFI_INTERVALO_REINTENTO_MS 30000  // Reintento si se pierde la conexión

/**************************** VARIABLES GLOBALES *****************************/
Medidas medidas = { NAN, NAN, false, NAN };

unsigned long tiempoUltimaLectura = 0;
unsigned long tiempoUltimaPantalla = 0;
unsigned long tiempoUltimaBateria = 0;

/**************************** FUNCIONES AUXILIARES ****************************/

/**
 * Inicializa la conexión WiFi en modo estación
 * Muestra el progreso de la conexión y la IP asignada.
 * Si no conecta en WIFI_TIMEOUT_CONEXION_MS, sigue sin red: los sensores, la
 * pantalla y la luz funcionan igual, y mantenerWiFi() reintenta después.
 * @return true si se ha conectado
 */
bool initWiFi() {
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
  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < WIFI_TIMEOUT_CONEXION_MS) {
    Serial.print('.');
    delay(500);
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No se pudo conectar al WiFi, se reintentará en segundo plano");
    return false;
  }

  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");        // RSSI es la intensidad de la señal WiFi
  Serial.println(WiFi.RSSI());
  return true;
}

/**
 * Comprueba la conexión WiFi y reintenta si se ha perdido. No bloquea.
 */
void mantenerWiFi() {
  static bool conectadoAntes = true;
  static unsigned long ultimoReintento = 0;
  bool conectado = (WiFi.status() == WL_CONNECTED);

  if (conectado) {
    if (!conectadoAntes) {
      Serial.print("WiFi reconectado con IP: ");
      Serial.println(WiFi.localIP());
    }
    conectadoAntes = true;
    return;
  }

  conectadoAntes = false;
  if (millis() - ultimoReintento >= WIFI_INTERVALO_REINTENTO_MS) {
    ultimoReintento = millis();
    Serial.println("WiFi desconectado, reintentando...");
    WiFi.reconnect();
  }
}

/**
 * Verifica los valores de los sensores y genera alertas si es necesario
 * @param m Últimas medidas leídas (los valores NAN indican sensor sin lectura)
 */
void verificarAlarmas(const Medidas& m) {
  // Verificar temperatura
  if (isnan(m.temperatura)) {
    Serial.println("¡ALARMA! El sensor de temperatura no responde");
  } else if (m.temperatura < ALARMA_TEMPERATURA_BAJA) {
    Serial.println("¡ALARMA! Temperatura demasiado baja");
  } else if (m.temperatura > ALARMA_TEMPERATURA_ALTA) {
    Serial.println("¡ALARMA! Temperatura demasiado alta");
  } else if (m.temperatura < AVISO_TEMPERATURA_BAJA || m.temperatura > AVISO_TEMPERATURA_ALTA) {
    Serial.println("AVISO: Temperatura fuera del rango ideal (22-26 °C)");
  }

  // Verificar conductividad
  if (!isnan(m.conductividad)) {
    if (m.conductividad > ALARMA_CONDUCTIVIDAD_ALTA) {
      Serial.println("¡ALARMA! Conductividad demasiado alta");
    } else if (m.conductividad > AVISO_CONDUCTIVIDAD_MEDIA) {
      Serial.println("AVISO: Conductividad por encima del rango ideal");
    }
  }

  // Verificar nivel de agua
  if (!m.nivelCorrecto) {
    Serial.println("¡ALARMA! Nivel de agua bajo");
  }

  // Verificar pila del comedero (solo si el módulo está habilitado)
  if (!isnan(m.voltajeBateria) && porcentajeBateria(m.voltajeBateria) < BATERIA_AVISO_PORCENTAJE) {
    Serial.println("AVISO: Pila del comedero baja, cambiarla pronto");
  }
}

/****************************** SETUP Y LOOP *********************************/

void setup() {
  Serial.begin(115200);          // Inicializar comunicación serial
  Serial.println("\n--- Iniciando sistema de monitoreo de acuario ---");

  // La luz se configura lo primero para fijarla apagada cuanto antes
  iniciarAtenuacion();
  iniciarPantalla();

  // Inicializar sensores
  setupSensorNivel();            // Sensor de nivel con filtro antirrebote
  medidas.nivelCorrecto = nivelCorrecto();  // Evita mostrar "BAJO" antes de la primera lectura
  setupSensorTemperatura();      // Inicializar sensor de temperatura DS18B20
  setupBateria();                // No hace nada mientras esté deshabilitado

  // Inicializar conexión WiFi y sincronización de hora
  initWiFi();
  setupNTP();

  // Forzar la primera lectura poco después de arrancar, cuando el buffer del
  // sensor TDS ya tenga muestras, en lugar de esperar 30 s
  tiempoUltimaLectura = millis() - INTERVALO_LECTURA_MS + RETARDO_PRIMERA_LECTURA_MS;
  tiempoUltimaBateria = millis() - INTERVALO_BATERIA_MS;

  Serial.println("Sistema de monitoreo de acuario iniciado");
}

void loop() {
  unsigned long ahora = millis();

  // Tareas continuas: ninguna bloquea, cada una controla su propio intervalo
  mantenerWiFi();
  muestrearConductividad();
  actualizarSensorNivel();
  actualizarAtenuacion();

  if (ahora - tiempoUltimaBateria >= INTERVALO_BATERIA_MS) {
    tiempoUltimaBateria = ahora;
    medidas.voltajeBateria = leerVoltajeBateria();  // NAN mientras esté deshabilitado
  }

  if (ahora - tiempoUltimaLectura >= INTERVALO_LECTURA_MS) {
    tiempoUltimaLectura = ahora;
    // 1. Mostrar la hora actual
    printLocalTime();

    // 2. Leer sensor de temperatura
    medidas.temperatura = leerTemperatura();  // Lectura en °C (NAN si falla)

    // 3. Leer sensor de conductividad, compensado con la temperatura
    medidas.conductividad = leerConductividad(medidas.temperatura);  // Lectura en ppm

    // 4. Leer sensor de nivel (valor ya filtrado)
    medidas.nivelCorrecto = nivelCorrecto();
    Serial.print("Nivel: ");
    Serial.println(medidas.nivelCorrecto ? "Correcto" : "Bajo");

    // 5. Verificar condiciones de alarma
    verificarAlarmas(medidas);

    // Línea separadora para mejorar legibilidad
    Serial.println("----------------------------------------");
  }

  // 6. Refrescar la pantalla (la hora y el brillo cambian más a menudo que las medidas)
  if (ahora - tiempoUltimaPantalla >= INTERVALO_PANTALLA_MS) {
    tiempoUltimaPantalla = ahora;
    mostrarEnPantalla(medidas, brilloLuzActual());
  }

  // Pausa breve: menor que el intervalo de muestreo del TDS (40 ms)
  delay(10);
}
