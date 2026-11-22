/*
 * Práctica 1 - Arduino (Sistemas Empotrados y Ubicuos)
 * ----------------------------------------------------
 * Termómetro con LM35 y dos LEDs de aviso:
 *   - LED verde : temperatura por debajo del umbral.
 *   - LED rojo  : temperatura igual o por encima del umbral,
 *                 o pulsador presionado (forzado manual).
 *
 * Cada segundo se envía por el puerto serie:
 *   tiempo, valor del ADC, tensión (mV) y temperatura (°C).
 *
 * Montaje (Proteus, ATmega328P a 5 V):
 *   LM35 (Vout) -> A0
 *   Pulsador    -> D4 (activo a nivel ALTO, resistencia pull-down externa)
 *   LED rojo    -> D3
 *   LED verde   -> D2
 */

// ---------------------------------------------------------------------------
// Pines (const: no cambian durante la ejecución y no ocupan RAM)
// ---------------------------------------------------------------------------
const uint8_t PIN_BOTON       = 4;
const uint8_t PIN_ROJO        = 3;
const uint8_t PIN_VERDE       = 2;
const uint8_t PIN_TEMP_SENSOR = A0;

// ---------------------------------------------------------------------------
// Parámetros del ADC y del sensor
// ---------------------------------------------------------------------------
const float VREF_MV          = 5000.0;  // Tensión de referencia del ADC (mV)
const float ADC_RESOLUCION   = 1024.0;  // ADC de 10 bits -> 0..1023
const float LM35_MV_POR_GRADO = 10.0;   // LM35: 10 mV/°C, 0 °C = 0 mV
const uint8_t NUM_MUESTRAS   = 10;      // Lecturas promediadas para filtrar ruido

// ---------------------------------------------------------------------------
// Lógica de control
// ---------------------------------------------------------------------------
const float UMBRAL_ROJO  = 24.0;  // °C a partir de los cuales se enciende el rojo
const float HISTERESIS   = 0.5;   // °C de margen para evitar parpadeos en el umbral

const unsigned long PERIODO_MUESTREO_MS = 1000;  // Lectura y envío cada 1 s
const unsigned long DURACION_INICIO_MS  = 2000;  // Prueba de LEDs al arrancar

// ---------------------------------------------------------------------------
// Variables de estado
// ---------------------------------------------------------------------------
unsigned long ultimoMuestreo = 0;   // Instante (ms) de la última lectura
bool temperaturaAlta = false;       // true si se ha superado el umbral
float tempDegrees = 0.0;            // Última temperatura medida (°C)


// ===========================================================================
// Funciones auxiliares
// ===========================================================================

// Lee el sensor NUM_MUESTRAS veces y devuelve la media del ADC (0..1023).
// Promediar reduce el ruido de la conversión analógica-digital.
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
// Ojo: se usan constantes float para evitar la división entera
// (3300/1024 en int daría 3 en lugar de 3.22).
float adcAMilivoltios(int valorAdc)
{
  return valorAdc * (VREF_MV / ADC_RESOLUCION);
}

// Convierte milivoltios a grados Celsius según la recta del LM35.
float milivoltiosAGrados(float mV)
{
  return mV / LM35_MV_POR_GRADO;
}

// Enciende un LED y apaga el otro.
void encenderLeds(bool rojo, bool verde)
{
  digitalWrite(PIN_ROJO,  rojo  ? HIGH : LOW);
  digitalWrite(PIN_VERDE, verde ? HIGH : LOW);
}

// Muestra por el terminal virtual una línea con todos los datos de la medida.
void imprimirMedida(int valorAdc, float mV, float grados)
{
  Serial.print(F(" Tiempo (sg) "));
  Serial.print(millis() / 1000);
  Serial.print(F(" tempSensor "));
  Serial.print(valorAdc);
  Serial.print(F(" tempVoltage "));
  Serial.print(mV);
  Serial.print(F(" tempDegrees "));
  Serial.println(grados);
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

  // Prueba de LEDs: ambos encendidos 2 s para indicar que arranca la aplicación
  encenderLeds(true, true);
  delay(DURACION_INICIO_MS);
  encenderLeds(false, false);

  // Forzamos una primera lectura inmediata en loop()
  ultimoMuestreo = millis() - PERIODO_MUESTREO_MS;
}


// ===========================================================================
// loop(): se ejecuta continuamente
// ---------------------------------------------------------------------------
// En vez de usar delay(1000), se comprueba con millis() si ha pasado el
// periodo de muestreo. Así el bucle nunca se bloquea y el pulsador se
// atiende al instante (con delay solo se leería una vez por segundo).
// ===========================================================================
void loop()
{
  unsigned long ahora = millis();

  // --- 1) Medida de temperatura cada PERIODO_MUESTREO_MS -------------------
  //     (la resta con unsigned long funciona aunque millis() se desborde)
  if (ahora - ultimoMuestreo >= PERIODO_MUESTREO_MS)
  {
    ultimoMuestreo = ahora;

    int   tempSensor  = leerSensorPromediado();
    float tempVoltage = adcAMilivoltios(tempSensor);
    tempDegrees       = milivoltiosAGrados(tempVoltage);

    imprimirMedida(tempSensor, tempVoltage, tempDegrees);

    // Comparación con histéresis:
    //   - se pasa a "alta" al llegar a UMBRAL_ROJO
    //   - se vuelve a "normal" al bajar de UMBRAL_ROJO - HISTERESIS
    // Evita que los LEDs parpadeen si la temperatura oscila justo en el umbral.
    if (!temperaturaAlta && tempDegrees >= UMBRAL_ROJO)
    {
      temperaturaAlta = true;
    }
    else if (temperaturaAlta && tempDegrees < UMBRAL_ROJO - HISTERESIS)
    {
      temperaturaAlta = false;
    }
  }

  // --- 2) Actualización de LEDs (en cada vuelta del bucle) -----------------
  // El pulsador tiene prioridad: mientras está pulsado se fuerza el rojo.
  bool botonPulsado = (digitalRead(PIN_BOTON) == HIGH);

  if (botonPulsado || temperaturaAlta)
  {
    encenderLeds(true, false);   // Rojo
  }
  else
  {
    encenderLeds(false, true);   // Verde
  }
}
