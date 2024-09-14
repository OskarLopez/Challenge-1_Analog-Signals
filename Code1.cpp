#include <Arduino.h>
#include <Adafruit_LiquidCrystal.h>

// Declaración de constantes
const int pinPulsadorAdquisicion = 2;
const int pinPulsadorProcesar = 3;
const int pinSignalInput = A0;

// Declaración de variables globales
bool adquisicionActiva = false;
bool procesarSignal = false;
int frecuencia = 0;
float amplitud = 0.0;
char formaOnda[20];

// Declaración de funciones
void setup() {
  // Inicializar pines de entrada y salida
  pinMode(pinPulsadorAdquisicion, INPUT);
  pinMode(pinPulsadorProcesar, INPUT);
  pinMode(pinSignalInput, INPUT);

  // Inicializar pantalla LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Informa2 Desafio 1");
}

void loop() {
  // Leer pulsador de adquisición
  if (digitalRead(pinPulsadorAdquisicion) == HIGH) {
    adquisicionActiva = true;
  }

  // Leer pulsador de procesar
  if (digitalRead(pinPulsadorProcesar) == HIGH) {
    procesarSignal = true;
  }

  // Adquisición de datos
  if (adquisicionActiva) {
    int* datos = new int[100]; // Reserva de memoria dinámica para almacenar los datos de la señal
    for (int i = 0; i < 100; i++) {
      datos[i] = analogRead(pinSignalInput);
    }

    // Procesar datos
    if (procesarSignal) {
      frecuencia = calcularFrecuencia(datos, 100);
      amplitud = calcularAmplitud(datos, 100);
      strcpy(formaOnda, identificarFormaOnda(datos, 100));

      // Mostrar resultados en pantalla LCD
      lcd.setCursor(0, 1);
      lcd.print("Frecuencia: ");
      lcd.print(frecuencia);
      lcd.print(" Hz");
      lcd.setCursor(0, 2);
      lcd.print("Amplitud: ");
      lcd.print(amplitud);
      lcd.print(" V");
      lcd.setCursor(0, 3);
      lcd.print("Forma de onda: ");
      lcd.print(formaOnda);

      // Liberar memoria dinámica
      delete[] datos;
    }
  }
}

// Funciones para calcular frecuencia, amplitud y forma de onda
int calcularFrecuencia(int* datos, int tamano) {
  // Implementar algoritmo para calcular frecuencia
  // ...
  return frecuencia;
}

float calcularAmplitud(int* datos, int tamano) {
  // Implementar algoritmo para calcular amplitud
  // ...
  return amplitud;
}

char* identificarFormaOnda(int* datos, int tamano) {
  // Implementar algoritmo para identificar forma de onda
  // ...
  return formaOnda;
}