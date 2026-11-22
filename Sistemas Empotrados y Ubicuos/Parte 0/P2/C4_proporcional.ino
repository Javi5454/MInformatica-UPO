/*
 * Práctica 2 - C4: Sistema de monitorización y control de temperatura (avanzado)
 * ------------------------------------------------------------------------------
 * Modifica C3 para que la acción de control sea PROPORCIONAL a la diferencia
 * entre la temperatura medida y la temperatura umbral:
 *
 *     velocidad (%) = KP * (temperatura - UMBRAL)     limitada a [0, 100]
 *
 *   - Por debajo del umbral la acción es negativa -> motor parado (0 %).
 *   - Por encima, el motor gira más rápido cuanto mayor es la diferencia,
 *     hasta saturar al 100 % cuando la diferencia llega a 100 / KP grados.
 *
 * El servo indica la velocidad impuesta: 0..100 %  ->  0..90º.
 * El pulsador, como en C3, fuerza el enfriamiento al 100 % mientras se pulsa.
 *
 * Constante de proporcionalidad: probar con KP = 20, 50 y 100.
 *
 * Montaje: el mismo que C3 (LM35 en A0, pulsador en D4, LED rojo en D3,
 * LED verde en D2, motor en D5 y servo en D9).
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
// Control proporcional
// ---------------------------------------------------------------------------
const float UMBRAL = 24.0;   // Temperatura de consigna (°C)
const float KP     = 20.0;   // Constante de proporcionalidad (% por °C).
                             // Probar con 20, 50 y 100.

const unsigned long PERIODO_MUESTREO_MS = 1000;  // Lectura y control cada 1 s
const unsigned long DURACION_INICIO_MS  = 2000;  // Prueba de LEDs al arrancar

// ---------------------------------------------------------------------------
// Actuadores
// ---------------------------------------------------------------------------
const uint8_t PWM_MAX       = 255;  // Valor PWM para el 100 % de velocidad
const uint8_t ANGULO_MAX    = 90;   // Ángulo del servo para el 100 %

// ---------------------------------------------------------------------------
// Variables de estado
// ---------------------------------------------------------------------------
Servo servo;                        // Objeto que controla el servomotor

unsigned long ultimoMuestreo = 0;   // Instante (ms) de la última lectura
uint8_t porcentajeTemp = 0;         // Acción de control calculada (0..100 %)
int16_t porcentajeAplicado = -1;    // Último % enviado a los actuadores
                                    // (-1 para forzar la primera actualización)


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
// Funciones auxiliares: control y actuadores
// ===========================================================================

// Ley de control proporcional: % de velocidad a partir de la temperatura.
// La acción se limita (satura) al rango físico del motor, 0..100 %.
uint8_t calcularAccionControl(float temperatura)
{
  float error  = temperatura - UMBRAL;   // > 0 si hace más calor del deseado
  float accion = KP * error;             // Acción proporcional al error

  if (accion <= 0.0)
  {
    return 0;                            // Por debajo del umbral: no enfriar
  }
  if (accion >= 100.0)
  {
    return 100;                          // Saturación: velocidad máxima
  }
  return (uint8_t)(accion + 0.5);        // Redondeo al entero más cercano
}

// Convierte un porcentaje (0..100) al valor PWM (0..255), redondeando (C1).
uint8_t porcentajeAPwm(uint8_t porcentaje)
{
  return (uint8_t)((porcentaje * (unsigned int)PWM_MAX + 50) / 100);
}

// Convierte un porcentaje (0..100) al ángulo del servo (0..90º), redondeando.
uint8_t porcentajeAAngulo(uint8_t porcentaje)
{
  return (uint8_t)((porcentaje * (unsigned int)ANGULO_MAX + 50) / 100);
}

// Enciende un LED y apaga el otro.
void encenderLeds(bool rojo, bool verde)
{
  digitalWrite(PIN_ROJO,  rojo  ? HIGH : LOW);
  digitalWrite(PIN_VERDE, verde ? HIGH : LOW);
}

// Aplica una velocidad (0..100 %) a todo el sistema de enfriamiento:
//   motor (PWM), servo indicador (0..90º) y LEDs (rojo si está enfriando).
void aplicarEnfriamiento(uint8_t porcentaje)
{
  analogWrite(PIN_MOTOR, porcentajeAPwm(porcentaje));
  servo.write(porcentajeAAngulo(porcentaje));
  encenderLeds(porcentaje > 0, porcentaje == 0);
}

// Muestra por el terminal virtual la medida y la acción de control.
void imprimirMedida(int valorAdc, float grados, uint8_t porcentaje)
{
  Serial.print(F(" Tiempo (sg) "));
  Serial.print(millis() / 1000);
  Serial.print(F(" tempSensor "));
  Serial.print(valorAdc);
  Serial.print(F(" tempDegrees "));
  Serial.print(grados);
  Serial.print(F("  error "));
  Serial.print(grados - UMBRAL);
  Serial.print(F("  accion "));
  Serial.print(porcentaje);
  Serial.print(F(" % (PWM "));
  Serial.print(porcentajeAPwm(porcentaje));
  Serial.print(F(", servo "));
  Serial.print(porcentajeAAngulo(porcentaje));
  Serial.println(F(" grados)"));
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

  // Estado inicial: motor parado y servo a 0º
  analogWrite(PIN_MOTOR, 0);
  servo.write(0);

  // Prueba de LEDs: ambos encendidos 2 s para indicar que arranca la aplicación
  encenderLeds(true, true);
  delay(DURACION_INICIO_MS);
  encenderLeds(false, true);

  Serial.print(F("Practica 2 - C4: control proporcional con KP = "));
  Serial.println(KP);

  // Forzamos una primera lectura inmediata en loop()
  ultimoMuestreo = millis() - PERIODO_MUESTREO_MS;
}


// ===========================================================================
// loop(): se ejecuta continuamente
// ---------------------------------------------------------------------------
// La temperatura se lee y la acción de control se recalcula cada
// PERIODO_MUESTREO_MS (sin delay); el pulsador se atiende en cada vuelta.
// ===========================================================================
void loop()
{
  unsigned long ahora = millis();

  // --- 1) Medida y cálculo de la acción de control -------------------------
  if (ahora - ultimoMuestreo >= PERIODO_MUESTREO_MS)
  {
    ultimoMuestreo = ahora;

    int   tempSensor  = leerSensorPromediado();
    float tempDegrees = milivoltiosAGrados(adcAMilivoltios(tempSensor));

    porcentajeTemp = calcularAccionControl(tempDegrees);

    imprimirMedida(tempSensor, tempDegrees, porcentajeTemp);
  }

  // --- 2) Aplicación a los actuadores --------------------------------------
  // El pulsador fuerza el 100 % mientras esté pulsado (como en C3).
  bool botonPulsado  = (digitalRead(PIN_BOTON) == HIGH);
  uint8_t porcentaje = botonPulsado ? 100 : porcentajeTemp;

  // Solo se actúa cuando cambia la acción, para no reenviar la misma orden
  // al motor y al servo en cada vuelta del bucle.
  if (porcentaje != porcentajeAplicado)
  {
    porcentajeAplicado = porcentaje;
    aplicarEnfriamiento(porcentaje);
  }
}
