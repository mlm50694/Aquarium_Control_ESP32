////////// CONFIGURACION DEL WIFI //////////
#include <WiFi.h>  //librería para la conexión wifi
#include "config.h"

////////// CONFIGURACION DEL SENSOR DE TEMPERATURA DS18B20 //////////
#include <OneWire.h> // Librería para la comunicación con dispositivos OneWire (como el sensor DS18B20)
#include <DallasTemperature.h> // Librería específica para manejar sensores de temperatura DS18B20 con OneWire

#define Temperature_Sensor 4  // Pin al que está conectado el DS18B20
OneWire oneWire(Temperature_Sensor); // Inicializa el bus OneWire en el pin definido anteriormente.
DallasTemperature sensors(&oneWire); // Crea el objeto 'sensors' para manejar los sensores DS18B20. Usa el objeto oneWire para inicializar la librería DallasTemperature

////////// CONFIGURACION DEL SENSOR DE CONDUCTIVIDAD TDS //////////
#define TDS_Sensor 34
#define VREF 3.3    
#define SAMPLES 10

////////// CONFIGURACION DEL SENSOR DE NIVEL //////////
#define Level_Sensor 25

////////// CONFIGURACION DE OTRAS VARIABLES DE ALARMA Y PROCESO //////////
#define Aviso_CONDUCTIVIDAD_media 200
#define Alarma_CONDUCTIVIDAD_alta 300
#define Alarma_TEMPERATURA_baja 20
#define Alarma_TEMPERATURA_alta 30


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initWiFi(){
WiFi.mode(WIFI_STA);  //el ESP32 actua en modo estación para conectarse a un punto de acceso.
WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup(){
  Serial.begin(115200);
  
  ///// Configuraciones del wifi
  initWiFi();
  Serial.print("RRSI: "); //RRSI es la intensidad de la señal wifi
  Serial.println(WiFi.RSSI());

  ///// Configuraciones de los sensores
  sensors.begin(); // Inicializa el sensor de temperatura
  pinMode(Level_Sensor, INPUT); //defina el pin del sensor de nivel como input
  ///////////////////////////////////////////////////////
   delay(1000);  // Esperar a que el ESP32 arranque completamente
}

void loop() {
  ////////// 1. LECTURA DEL SENSOR DE TEMPERATURA ////////// 
  // Por defecto el sensor usa 12 bits, que es lo más óptimo para este proyecto
  sensors.requestTemperatures(); // Solicita la temperatura al sensor
  float temperatura = sensors.getTempCByIndex(0); // Devuelve la temperatura en grados Celsius como un número float. Al solo existir un sensor DS18B20 conectado, siempre usará el índice 0
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");

  ////////// 2. LECTURA DEL SENSOR DE CONDUCTIVIDAD ////////// 
  int Conductividad = 0; //declara una variable "Conductividad"

  for (int i = 0; i < SAMPLES; i++) {
    Conductividad += analogRead(TDS_Sensor); //El bucle se utiliza para promediar las lecturas del sensor de conductividad para obtener un valor más estable y preciso.
    delay(10); //Los sensores analógicos, como el sensor de TDS, pueden ser ruidosos y sensibles a pequeñas fluctuaciones o interferencias
  }
  Conductividad = Conductividad / SAMPLES; //Calculo del promedio de las lecturas realizadas durante el bucle anterior
  float voltage = Conductividad * (VREF / 4095.0); //Aquí se convierte el valor de la lectura del sensor a un voltaje real (4095 es el valor máximo de conversión del ADC de 12 bits. Por ejemplo, si el ADC está alimentado a 3.3V, VREF sería 3.3V)
  float tdsValue = (voltage / VREF) * 1000; 
  Serial.print("TDS: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");

  ////////// 3. LECTURA DEL SENSOR DE NIVEL ////////// 
  bool Nivel = digitalRead(Level_Sensor);
  Serial.print("Nivel: ");
  if (Nivel == 0){
    Serial.println("Nivel Bajo");
  }else{
    Serial.println("Nivel Correcto");
  }
  ////////// . VERIFICACIÓN DEL UMBRAL DE TDS ////////// 
  //if (tdsValue >= ALARMA_CONDUCTIVIDAD_alta) {
    // Activar la alarma
    //digitalWrite(BUZZER_PIN, HIGH);  // Encender el buzzer
    //digitalWrite(LED_PIN, HIGH);     // Encender el LED
  //} else {
    // Desactivar la alarma si el valor de TDS está por debajo del umbral
    //digitalWrite(BUZZER_PIN, LOW);
    //digitalWrite(LED_PIN, LOW);

  ///////////////////////////////////////////////////////
  delay(1000); // Lectura cada segundo
}


