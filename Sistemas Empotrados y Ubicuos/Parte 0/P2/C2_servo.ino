/*
 * Práctica 2 - C2: Sistema actuador "SERVO" (Sistemas Empotrados y Ubicuos)
 * --------------------------------------------------------------------------
 * Secuencia de pruebas del servomotor (Activar con cierto ángulo):
 *
 *     0º  ->  45º  ->  90º  ->  45º  ->  0º      (2 s en cada posición)
 *
 * La secuencia se repite indefinidamente y cada cambio de posición se
 * informa por el puerto serie (Virtual Terminal).
 *
 * Montaje (Proteus, ATmega328P a 16 MHz):
 *   D9 (IO9, PB1/OC1A)  -> entrada de control (pulso) del servo MOTOR-PWMSERVO
 *   +5 V del Arduino    -> alimentación (+) del servo
 *   GND                 -> tierra (-) del servo
 *   Condensador 100 uF  -> en paralelo con la alimentación del servo
 *                          (terminal + a +5 V, terminal - a GND)
 *
 *   - El servo se alimenta a 5 V desde el Arduino, NO desde la batería de 9 V
 *     del motor.
 *   - El condensador de desacoplo absorbe el pico de corriente que pide el
 *     servo al empezar a moverse, evitando caídas de tensión en el Arduino.
 *
 * El motor de C1 sigue conectado al pin 5: se deja apagado durante la prueba.
 */

#include <Servo.h>   // Librería estándar de Arduino para servomotores

// ---------------------------------------------------------------------------
// Pines
// ---------------------------------------------------------------------------
// La librería Servo genera los pulsos con el Timer1, por lo que mientras esté
// en uso no se puede usar analogWrite() en los pines 9 y 10. Por eso el motor
// (C1) está en el pin 5, que depende del Timer0.
const uint8_t PIN_SERVO = 9;
const uint8_t PIN_MOTOR = 5;    // Motor de C1 (se mantiene apagado)

// ---------------------------------------------------------------------------
// Temporización
// ---------------------------------------------------------------------------
const unsigned long DURACION_PASO_MS = 2000;   // Cada posición se mantiene 2 s

// ---------------------------------------------------------------------------
// Objetos y variables globales
// ---------------------------------------------------------------------------
Servo servo;   // Objeto que controla el servomotor


// ===========================================================================
// Funciones auxiliares
// ===========================================================================

// Lleva el servo al ángulo indicado (0..180 grados) e informa por el serie.
// servo.write() se encarga de generar el pulso adecuado: un pulso cada 20 ms
// cuya duración (entre 544 us para 0º y 2400 us para 180º) fija la posición.
void moverServo(uint8_t angulo)
{
  servo.write(angulo);

  Serial.print(F(" Tiempo (sg) "));
  Serial.print(millis() / 1000);
  Serial.print(F("  Servo a "));
  Serial.print(angulo);
  Serial.println(F(" grados"));
}

// Secuencia de pruebas: 0º / 45º / 90º / 45º / 0º, 2 s en cada posición.
void pruebaServo()
{
  Serial.println(F("--- Prueba del servo: 0 / 45 / 90 / 45 / 0 grados ---"));

  // Lista de ángulos a aplicar, cada uno durante DURACION_PASO_MS
  const uint8_t angulos[] = {0, 45, 90, 45, 0};
  const uint8_t numPasos = sizeof(angulos) / sizeof(angulos[0]);

  for (uint8_t i = 0; i < numPasos; i++)
  {
    moverServo(angulos[i]);
    delay(DURACION_PASO_MS);
  }
}


// ===========================================================================
// setup(): se ejecuta una sola vez al arrancar
// ===========================================================================
void setup()
{
  // Comunicación serie PC-Arduino (debe coincidir con el Virtual Terminal)
  Serial.begin(9600);

  // Motor de C1: salida a nivel bajo para que no arranque (puerta del MOSFET
  // sin dejar al aire)
  pinMode(PIN_MOTOR, OUTPUT);
  digitalWrite(PIN_MOTOR, LOW);

  // Servo: se asocia el objeto al pin y se lleva a la posición inicial (0º)
  pinMode(PIN_SERVO, OUTPUT);
  servo.attach(PIN_SERVO);
  servo.write(0);

  Serial.println(F("Practica 2 - C2: prueba del sistema actuador SERVO"));
}


// ===========================================================================
// loop(): se ejecuta continuamente
// ===========================================================================
void loop()
{
  pruebaServo();   // 10 s
}
