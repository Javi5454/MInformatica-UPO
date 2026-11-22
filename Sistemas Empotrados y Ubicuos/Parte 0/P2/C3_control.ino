/*
 * Práctica 2 - C3: Sistema de monitorización y control de temperatura (base)
 * --------------------------------------------------------------------------
 * Extiende el termómetro de la Práctica 1 (LM35 + LEDs + pulsador) con los
 * actuadores de C1 (motor) y C2 (servo) para formar un sistema de
 * enfriamiento todo/nada:
 *
 *   - Temperatura < umbral y botón sin pulsar:
 *       motor APAGADO, servo a 0º  (enfriamiento parado), LED verde.
 *   - Temperatura >= umbral o botón pulsado:
 *       motor al 100 %, servo a 90º (enfriamiento en marcha), LED rojo.
 *
 * Cada segundo se envía por el puerto serie la medida y el estado del sistema.
 *
 * Montaje (Proteus, ATmega328P a 16 MHz):
 *   LM35 (Vout) -> A0
 *   Pulsador    -> D4 (activo a nivel ALTO, resistencia pull-down externa)
 *   LED rojo    -> D3
 *   LED verde   -> D2
 *   Motor       -> D5 (puerta del MOSFET 2N7000, ver C1)
 *   Servo       -> D9 (control del MOTOR-PWMSERVO, ver C2)
 */

#include <Servo.h>   // Librería estándar de Arduino para servomotores

// ---------------------------------------------------------------------------
// Pines
// ---------------------------------------------------------------------------
const uint8_t PIN_BOTON       = 4;
const uint8_t PIN_ROJO        = 3;
const uint8_t PIN_VERDE       = 2;
const uint8_t PIN_TEMP_SENSOR = A0;
const uint8_t PIN_MOTOR       = 5;   // Pin PWM del Timer0 (no le afecta Servo)
const uint8_t PIN_SERVO       = 9;   // La librería Servo usa el Timer1

// ---------------------------------------------------------------------------
// Parámetros del ADC y del sensor (igual que en la Práctica 1)
// ---------------------------------------------------------------------------
const float VREF_MV           = 5000.0;  // Tensión de referencia del ADC (mV)
const float ADC_RESOLUCION    = 1024.0;  // ADC de 10 bits -> 0..1023
const float LM35_MV_POR_GRADO = 10.0;    // LM35: 10 mV/°C, 0 °C = 0 mV
const uint8_t NUM_MUESTRAS    = 10;      // Lecturas promediadas para filtrar ruido

// ---------------------------------------------------------------------------
// Lógica de control
// ---------------------------------------------------------------------------
const float UMBRAL       = 24.0;  // °C a partir de los cuales se enfría
const float HISTERESIS   = 0.5;   // °C de margen para no conmutar sin parar
                                  // en torno al umbral (0 = sin histéresis)

const unsigned long PERIODO_MUESTREO_MS = 1000;  // Lectura y envío cada 1 s
const unsigned long DURACION_INICIO_MS  = 2000;  // Prueba de LEDs al arrancar

// ---------------------------------------------------------------------------
// Actuadores
// ---------------------------------------------------------------------------
const uint8_t PWM_MOTOR_MAX  = 255;  // 100 % de la velocidad máxima
const uint8_t ANGULO_PARADO  = 0;    // Servo: enfriamiento parado
const uint8_t ANGULO_MARCHA  = 90;   // Servo: enfriamiento en marcha

// ---------------------------------------------------------------------------
// Variables de estado
// ---------------------------------------------------------------------------
Servo servo;                        // Objeto que controla el servomotor

unsigned long ultimoMuestreo = 0;   // Instante (ms) de la última lectura
bool temperaturaAlta = false;       // true si se ha superado el umbral
bool enfriando = false;             // Estado actual de los actuadores
float tempDegrees = 0.0;            // Última temperatura medida (°C)


// ===========================================================================
// Funciones auxiliares: sensor (Práctica 1)
// ===========================================================================

// Lee el sensor NUM_MUESTRAS veces y devuelve la media del ADC (0..1023).
int leerSensorPromediado()
{
  long suma = 0;
  for (uint8_t i = 0; i < NUM_MUESTRAS; i++)
  {
    suma += analogRead(PIN_TEMP_SENSOR);
  }
  return suma / NUM_MUESTRAS;
}

// Convierte un valor del ADC a milivoltios.
float adcAMilivoltios(int valorAdc)
{
  return valorAdc * (VREF_MV / ADC_RESOLUCION);
}

// Convierte milivoltios a grados Celsius según la recta del LM35.
float milivoltiosAGrados(float mV)
{
  return mV / LM35_MV_POR_GRADO;
}


// ===========================================================================
// Funciones auxiliares: actuadores
// ===========================================================================

// Enciende un LED y apaga el otro.
void encenderLeds(bool rojo, bool verde)
{
  digitalWrite(PIN_ROJO,  rojo  ? HIGH : LOW);
  digitalWrite(PIN_VERDE, verde ? HIGH : LOW);
}

// Activa o desactiva el sistema de enfriamiento completo:
//   motor (C1) + servo indicador (C2) + LEDs (Práctica 1).
void aplicarEnfriamiento(bool activo)
{
  if (activo)
  {
    analogWrite(PIN_MOTOR, PWM_MOTOR_MAX);   // Motor al 100 %
    servo.write(ANGULO_MARCHA);              // Servo a 90º
    encenderLeds(true, false);               // LED rojo
  }
  else
  {
    analogWrite(PIN_MOTOR, 0);               // Motor parado
    servo.write(ANGULO_PARADO);              // Servo a 0º
    encenderLeds(false, true);               // LED verde
  }
}

// Muestra por el terminal virtual la medida y el estado del sistema.
void imprimirMedida(int valorAdc, float mV, float grados)
{
  Serial.print(F(" Tiempo (sg) "));
  Serial.print(millis() / 1000);
  Serial.print(F(" tempSensor "));
  Serial.print(valorAdc);
  Serial.print(F(" tempVoltage "));
  Serial.print(mV);
  Serial.print(F(" tempDegrees "));
  Serial.print(grados);
  Serial.print(F("  Enfriamiento "));
  Serial.println(enfriando ? F("ON (motor 100 %, servo 90)") : F("OFF (motor 0 %, servo 0)"));
}


// ===========================================================================
// setup(): se ejecuta una sola vez al arrancar
// ===========================================================================
void setup()
{
  // Comunicación serie PC-Arduino (debe coincidir con el Virtual Terminal)
  Serial.begin(9600);

  // Entradas y salidas digitales
  pinMode(PIN_BOTON, INPUT);
  pinMode(PIN_ROJO,  OUTPUT);
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);

  // Servo asociado a su pin
  servo.attach(PIN_SERVO);

  // Estado inicial: enfriamiento parado (motor apagado, servo a 0º)
  analogWrite(PIN_MOTOR, 0);
  servo.write(ANGULO_PARADO);

  // Prueba de LEDs: ambos encendidos 2 s para indicar que arranca la aplicación
  encenderLeds(true, true);
  delay(DURACION_INICIO_MS);

  aplicarEnfriamiento(false);

  // Forzamos una primera lectura inmediata en loop()
  ultimoMuestreo = millis() - PERIODO_MUESTREO_MS;
}


// ===========================================================================
// loop(): se ejecuta continuamente
// ---------------------------------------------------------------------------
// Igual que en la Práctica 1, no se usa delay(): la temperatura se lee cada
// PERIODO_MUESTREO_MS comprobando millis(), y el pulsador se atiende en cada
// vuelta del bucle, sin esperas.
// ===========================================================================
void loop()
{
  unsigned long ahora = millis();

  // --- 1) Medida de temperatura cada PERIODO_MUESTREO_MS -------------------
  if (ahora - ultimoMuestreo >= PERIODO_MUESTREO_MS)
  {
    ultimoMuestreo = ahora;

    int   tempSensor  = leerSensorPromediado();
    float tempVoltage = adcAMilivoltios(tempSensor);
    tempDegrees       = milivoltiosAGrados(tempVoltage);

    // Comparación con histéresis:
    //   - se pasa a "alta" al llegar a UMBRAL
    //   - se vuelve a "normal" al bajar de UMBRAL - HISTERESIS
    // Evita que el motor y el servo se enciendan y apaguen continuamente si
    // la temperatura oscila justo en el umbral.
    if (!temperaturaAlta && tempDegrees >= UMBRAL)
    {
      temperaturaAlta = true;
    }
    else if (temperaturaAlta && tempDegrees < UMBRAL - HISTERESIS)
    {
      temperaturaAlta = false;
    }

    imprimirMedida(tempSensor, tempVoltage, tempDegrees);
  }

  // --- 2) Decisión de control (en cada vuelta del bucle) -------------------
  // El pulsador fuerza el enfriamiento mientras esté pulsado.
  bool botonPulsado = (digitalRead(PIN_BOTON) == HIGH);
  bool debeEnfriar  = botonPulsado || temperaturaAlta;

  // Solo se actúa cuando cambia el estado, para no reenviar órdenes iguales
  // al motor y al servo en cada vuelta y poder informar del cambio.
  if (debeEnfriar != enfriando)
  {
    enfriando = debeEnfriar;
    aplicarEnfriamiento(enfriando);

    // Se indica también el motivo de la activación (botón o temperatura)
    Serial.print(F(" >> Enfriamiento "));
    if (enfriando)
    {
      Serial.print(F("ACTIVADO"));
      Serial.println(botonPulsado ? F(" (boton)") : F(" (temperatura)"));
    }
    else
    {
      Serial.println(F("DESACTIVADO"));
    }
  }
}
