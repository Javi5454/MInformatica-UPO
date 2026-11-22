/*
 * Práctica 2 - C1: Sistema actuador "MOTOR" (Sistemas Empotrados y Ubicuos)
 * --------------------------------------------------------------------------
 * Secuencia de pruebas del motor de corriente continua en sus dos modos:
 *
 *   MODO 1 - Activar (velocidad máxima) / Desactivar  -> digitalWrite()
 *     2 s apagado  ->  2 s al 100 %  ->  2 s apagado
 *
 *   MODO 2 - Activar (punto de trabajo) / Desactivar  -> analogWrite() (PWM)
 *     2 s apagado  ->  2 s al 25 %  ->  2 s al 50 %  ->  2 s al 100 %  ->  2 s apagado
 *
 * La secuencia completa (modo 1 + modo 2) se repite indefinidamente y cada
 * cambio de estado se informa por el puerto serie (Virtual Terminal).
 *
 * Montaje (Proteus, ATmega328P a 16 MHz):
 *   D5 (IO5, PD5/OC0B, PWM) -> Gate del MOSFET 2N7000
 *   Drain 2N7000            -> terminal (-) del motor y ánodo del diodo
 *   Source 2N7000           -> GND
 *   +9 V (generador DC)     -> terminal (+) del motor y cátodo del diodo
 *
 *   - El MOSFET actúa como interruptor: el Arduino no puede dar la corriente
 *     que necesita el motor, así que solo controla la puerta (G) y la
 *     corriente del motor circula de D a S desde la batería de 9 V.
 *   - El diodo en paralelo (en inverso) protege el circuito de la corriente
 *     inducida por la bobina del motor al cortarle la alimentación.
 */

// ---------------------------------------------------------------------------
// Pines
// ---------------------------------------------------------------------------
// Debe ser un pin PWM (en el ATmega328P: 3, 5, 6, 9, 10, 11), ya que en el
// modo 2 se usa analogWrite(). El pin 5 sale del comparador OC0B del Timer0.
const uint8_t PIN_MOTOR = 5;

// ---------------------------------------------------------------------------
// Temporización
// ---------------------------------------------------------------------------
const unsigned long DURACION_PASO_MS = 2000;   // Cada paso de la prueba dura 2 s

// ---------------------------------------------------------------------------
// PWM
// ---------------------------------------------------------------------------
// analogWrite() admite valores de 0 (motor parado) a 255 (velocidad máxima).
// El valor fija el ciclo de trabajo de la señal PWM (~980 Hz en el pin 5):
// el motor no sigue cambios tan rápidos y responde a la tensión media,
// es decir, aproximadamente a  9 V * valor / 255.
const uint8_t PWM_MAX = 255;


// ===========================================================================
// Funciones auxiliares
// ===========================================================================

// Convierte un porcentaje de velocidad (0..100) al valor PWM (0..255).
// Se redondea al entero más cercano: 25 % -> 64, 50 % -> 128, 100 % -> 255.
uint8_t porcentajeAPwm(uint8_t porcentaje)
{
  if (porcentaje > 100)
  {
    porcentaje = 100;
  }
  // Se usa aritmética entera de 16 bits: 100 * 255 = 25500 cabe en un int.
  return (uint8_t)((porcentaje * (unsigned int)PWM_MAX + 50) / 100); // Se suma 50 para redondear
}

// MODO 1: encendido a velocidad máxima o apagado (salida digital todo/nada).
void motorDigital(bool encendido)
{
  digitalWrite(PIN_MOTOR, encendido ? HIGH : LOW);

  Serial.print(F(" Tiempo (sg) "));
  Serial.print(millis() / 1000);
  Serial.print(F("  [Modo 1] Motor "));
  Serial.println(encendido ? F("ENCENDIDO (100 %)") : F("APAGADO"));
}

// MODO 2: fija la velocidad del motor como porcentaje de la máxima (PWM).
void motorVelocidad(uint8_t porcentaje)
{
  uint8_t valorPwm = porcentajeAPwm(porcentaje);
  analogWrite(PIN_MOTOR, valorPwm);

  Serial.print(F(" Tiempo (sg) "));
  Serial.print(millis() / 1000);
  Serial.print(F("  [Modo 2] Motor al "));
  Serial.print(porcentaje);
  Serial.print(F(" % (PWM = "));
  Serial.print(valorPwm);
  Serial.println(F(")"));
}

// Secuencia de pruebas del MODO 1: apagado / 100 % / apagado.
void pruebaModoDigital()
{
  Serial.println(F("--- MODO 1: Activar (velocidad maxima) / Desactivar ---"));

  motorDigital(false);
  delay(DURACION_PASO_MS);

  motorDigital(true);
  delay(DURACION_PASO_MS);

  motorDigital(false);
  delay(DURACION_PASO_MS);
}

// Secuencia de pruebas del MODO 2: apagado / 25 % / 50 % / 100 % / apagado.
void pruebaModoPwm()
{
  Serial.println(F("--- MODO 2: Activar (punto de trabajo) / Desactivar ---"));

  // Lista de velocidades a aplicar, cada una durante DURACION_PASO_MS
  const uint8_t pasos[] = {0, 25, 50, 100, 0};
  const uint8_t numPasos = sizeof(pasos) / sizeof(pasos[0]);

  for (uint8_t i = 0; i < numPasos; i++)
  {
    motorVelocidad(pasos[i]);
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

  // El pin del motor es una salida
  pinMode(PIN_MOTOR, OUTPUT);

  // Inicialización: motor parado
  digitalWrite(PIN_MOTOR, LOW);

  Serial.println(F("Practica 2 - C1: prueba del sistema actuador MOTOR"));
}


// ===========================================================================
// loop(): se ejecuta continuamente
// ---------------------------------------------------------------------------
// Aquí sí se usa delay(): la prueba es una secuencia fija en el tiempo y no
// hay que atender ninguna entrada mientras tanto.
// ===========================================================================
void loop()
{
  pruebaModoDigital();   // 6 s
  pruebaModoPwm();       // 10 s
}
