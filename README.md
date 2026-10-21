# 🐠 Control IoT de pecera con ESP32

Controlador para una pecera de agua dulce (guppys y neones) basado en un ESP32.
Mide temperatura, conductividad (TDS) y nivel de agua, simula amanecer y anochecer
con la lámpara y muestra el estado en una pantalla OLED. Los comentarios del código
están en español.

## Hardware y pines

| Componente | Modelo | GPIO | Notas |
|---|---|---|---|
| Placa | ESP32 HW-394 (ESP-WROOM-32) | — | USB-C |
| Temperatura | Sonda DS18B20 sumergible | 4 | OneWire, resistencia pull-up de 4,7 kΩ entre DATA y 3,3 V |
| Conductividad | TDS Meter V1.0 (compatible DFRobot SEN0244) | 34 | ADC1, salida 0–2,3 V, rango 0–1000 ppm |
| Nivel | XKC-Y25-V (capacitivo, sin contacto) | 25 | ⚠ Ver notas de hardware |
| Pantalla | OLED 0,96" SSD1306 128×64 bicolor | 21 (SDA) / 22 (SCL) | I2C, dirección 0x3C |
| Lámpara | LED 5 V · 0,35 A + MOSFET IRLZ44N | 15 | PWM 5 kHz, 13 bits. ⚠ Ver notas de hardware |
| Pila del comedero | Divisor resistivo | 35 | Fase futura, desactivada en el código |

## ⚠ Notas de hardware pendientes

### 1. Sensor de nivel: 5 V en la entrada del ESP32

El XKC-Y25-V necesita **5–24 V** de alimentación y su salida en nivel alto es igual
a la tensión de alimentación. Alimentado a 5 V, entrega **5 V al GPIO25**, y los
pines del ESP32 admiten como máximo **3,6 V**. Puede funcionar un tiempo, pero
degrada el pin. Hay dos soluciones:

- **Divisor de tensión en el cable amarillo** (recomendado, coste ~0 €):

  ```
  Amarillo (OUT) ── 10 kΩ ──┬── GPIO25
                            └── 20 kΩ ── GND
  ```
  5 V × 20/30 = 3,33 V. El pull-down interno que activa el código (~45 kΩ) queda en
  paralelo con los 20 kΩ y la tensión baja a ~2,9 V, que el ESP32 sigue leyendo
  como nivel alto.

- **Cambiar al modelo XKC-Y25-NPN**, cuya salida es de colector abierto, y usar
  `INPUT_PULLUP` a 3,3 V (habría que invertir `NIVEL_ACTIVO_ALTO`).

Recordatorio del fabricante: el cable **negro (MODO) nunca va a GND**. Al aire
funciona en modo normal (ALTO = hay agua); unido al azul invierte la lógica.

### 2. Lámpara en GPIO15

GPIO15 es un pin de arranque del ESP32 y emite una señal PWM durante el boot,
así que la lámpara puede parpadear en cada reinicio. Cuando se rehaga el
cableado (o en la PCB):

- Mover la lámpara a **GPIO26** y cambiar `LED_PIN` en `Atenuacion_Luz.h`.
- Añadir una resistencia de **10 kΩ entre la puerta y el surtidor del MOSFET**,
  para que la lámpara quede apagada mientras el ESP32 arranca o se reinicia.

### 3. Pila del comedero automático (fase futura)

El código ya está escrito en `Funcion_Bateria.*` pero desactivado
(`BATERIA_HABILITADA false`). Para activarlo:

1. Montar un divisor 100 kΩ / 100 kΩ entre el positivo de la pila y GND, con la
   toma intermedia en GPIO35 y un condensador de 100 nF a GND.
2. Unir el negativo de la pila al GND del ESP32.
3. Ajustar `BATERIA_V_LLENA` y `BATERIA_V_VACIA` al tipo de pila real (ahora tienen
   valores provisionales para 2 pilas AA alcalinas).
4. Opcional: para no descargar la pila con el divisor (~15 µA), conectarlo solo
   durante la medida mediante un transistor gobernado por `BATERIA_PIN_ACTIVACION`.
5. Poner `BATERIA_HABILITADA` a `true`.

## Configuración

1. Instalar el soporte de placas **ESP32 de Espressif** (versión 2.x o 3.x; el
   código es compatible con las dos).
2. Instalar las librerías: `OneWire`, `DallasTemperature`, `Adafruit SSD1306` y
   `Adafruit GFX Library`.
3. Copiar `config.example.h` como `config.h` y rellenar los datos del WiFi.
   `config.h` no se sube al repositorio.
4. La IP estática (192.168.1.143) se define en `initWiFi()`.

## Funcionamiento

| Tarea | Frecuencia | Detalle |
|---|---|---|
| Muestreo del TDS | Cada 40 ms | Mediana de las últimas 30 muestras |
| Sensor de nivel | Continuo | Un cambio se acepta tras 5 s estable (filtra el oleaje) |
| Atenuación de la luz | Cada 200 ms | Según la hora NTP |
| Lectura de sensores y alarmas | Cada 30 s | Por el puerto serie a 115200 baudios |
| Pantalla OLED | Cada 1 s | Hora y luz en la franja amarilla; medidas en la azul |
| Pila del comedero | Cada 10 min | Solo si está habilitada |

Ninguna tarea bloquea el bucle principal. Sin WiFi o sin hora NTP, el sistema
sigue midiendo y reintenta la conexión cada 30 s. Si la OLED no responde, se
continúa sin ella.

### Conductividad (TDS)

El valor se calcula con la curva de calibración del fabricante (voltaje → EC en
µS/cm), se compensa a 25 °C con la temperatura del DS18B20 y se convierte a ppm
con un factor de 0,5 (agua dulce).

**Calibración** con una solución patrón (por ejemplo, 707 ppm / 1413 µS/cm):
sumergir la sonda, anotar el TDS que muestra el monitor serie y ajustar
`TDS_FACTOR_K = valor_patrón / valor_medido` en `Funcion_Conductividad.h`.

### Atenuación de la luz

Horario por defecto: amanecer de 13:00 a 14:00 y anochecer de 20:00 a 21:00
(`Atenuacion_Luz.h`). El brillo se calcula a partir de la hora, así que tras un
reinicio la luz vuelve directamente a su nivel. Se aplica corrección gamma para
que la rampa se perciba uniforme. `configurarHorarioLuz()` permite cambiar el
horario en tiempo de ejecución; se usará desde la futura app web.

### Umbrales de alarma

| Parámetro | Aviso | Alarma |
|---|---|---|
| Temperatura | Fuera de 22–26 °C | < 20 °C o > 28 °C, o sensor sin respuesta |
| TDS | > 150 ppm | > 300 ppm |
| Nivel | — | Nivel bajo |
| Pila del comedero | < 20 % | — |

Los umbrales de TDS deben revisarse tras calibrar la sonda.

## Próximas fases

- Envío de datos por MQTT a un servidor local y almacenamiento en InfluxDB.
- Alarmas al móvil por Telegram.
- App web en el servidor local para ver los datos y modificar los setpoints
  (horario de luz, umbrales), con persistencia en la memoria NVS del ESP32.
- PCB definitiva.
