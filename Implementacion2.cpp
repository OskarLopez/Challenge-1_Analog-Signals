#include <Adafruit_LiquidCrystal.h>

// Definir pantalla LCD I2C
Adafruit_LiquidCrystal lcd(0); // LCD I2C

// Configuración del tamaño del buffer de muestras
const int buffer_size = 100;  // Cantidad de muestras a capturar
int* buffer;  // Arreglo dinámico para almacenar las muestras

// Variables dinámicas para los resultados
float* frecuencia;
float* amplitud;
char* forma_onda;

// Variables internas para medir frecuencia y amplitud
unsigned long* tiempo_inicio;
unsigned long* tiempo_fin;

// Definir los pines de los pulsadores
const int pulsadorInicio = 2;  // Pulsador para iniciar la adquisición
const int pulsadorMostrar = 3; // Pulsador para mostrar los datos

// Variables para controlar los pulsadores y estados
bool adquiriendo = false;
bool mostrar_datos = false;

// Variables para el tiempo de actualización y adquisición
unsigned long tiempo_ultima_actualizacion_lcd = 0;
unsigned long lcd_refresh_delay = 500;  // Actualizar el LCD cada 500ms

// Variables para almacenar la cantidad de muestras capturadas
int muestras_capturadas = 0;

void setup() {
  // Inicialización de la pantalla LCD
  lcd.begin(16, 2);
  lcd.setBacklight(LOW);
  Serial.begin(9600);

  // Configuración de los pulsadores
  pinMode(pulsadorInicio, INPUT);
  pinMode(pulsadorMostrar, INPUT);

  // Inicialización dinámica de las variables
  buffer = (int*)malloc(buffer_size * sizeof(int));  // Almacenar las muestras
  frecuencia = (float*)malloc(sizeof(float));
  amplitud = (float*)malloc(sizeof(float));
  forma_onda = (char*)malloc(20 * sizeof(char));  // 20 caracteres para la forma de onda

  tiempo_inicio = (unsigned long*)malloc(sizeof(unsigned long));
  tiempo_fin = (unsigned long*)malloc(sizeof(unsigned long));

  // Inicializar valores
  *tiempo_inicio = 0;
  *tiempo_fin = 0;

  // Mensaje inicial en el LCD
  lcd.setCursor(0, 0);
  lcd.print("Presione Inicio");
}

void loop() {
  // Leer el estado del pulsador para iniciar la adquisición
  if (digitalRead(pulsadorInicio) == HIGH && !adquiriendo) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Adquiriendo...");

    adquiriendo = true;  // Iniciar la adquisición
    muestras_capturadas = 0;  // Reiniciar el contador de muestras
  }

  // Si la adquisición está activa, capturar las muestras y mostrar en el Monitor Serial
  if (adquiriendo) {
    capturarMuestras();
    
    // Mostrar los datos de la señal en el Monitor Serial durante la adquisición
    if (muestras_capturadas > 0 && muestras_capturadas < buffer_size) {
      Serial.print("Muestra: ");
      Serial.print(muestras_capturadas);
      Serial.print(" Valor: ");
      Serial.println(buffer[muestras_capturadas - 1]);  // Mostrar la última muestra capturada
    }
    
    // Si se llenó el buffer, detener la adquisición
    if (muestras_capturadas >= buffer_size) {
      adquiriendo = false;  // Detener la adquisición
      procesarDatos();  // Procesar los datos capturados
    }
  }

  // Leer el estado del pulsador para mostrar los datos
  if (digitalRead(pulsadorMostrar) == HIGH && !adquiriendo) {
    mostrar_datos = true;  // Indicar que se deben mostrar los datos
  }

  // Si se ha solicitado mostrar los datos
  if (mostrar_datos) {
    mostrarResultados();  // Mostrar resultados en la pantalla y en el monitor serie
    mostrar_datos = false;  // Resetear la bandera
    // Reanudar la adquisición
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Presione Inicio");
  }
}

// Función para capturar las muestras de la señal
void capturarMuestras() {
  if (muestras_capturadas < buffer_size) {
    buffer[muestras_capturadas] = analogRead(A0);  // Capturar la señal del generador
    muestras_capturadas++;  // Incrementar el contador de muestras
  }
}

// Función para procesar los datos capturados
void procesarDatos() {
  int valor_min = 1023;
  int valor_max = 0;

  // Calcular frecuencia y amplitud usando el buffer de muestras
  for (int i = 0; i < buffer_size; i++) {
    if (buffer[i] < valor_min) valor_min = buffer[i];
    if (buffer[i] > valor_max) valor_max = buffer[i];

    // Detección de cruce por cero para calcular la frecuencia
    if (i > 0 && buffer[i - 1] < 512 && buffer[i] >= 512) {
      *tiempo_fin = micros();
      unsigned long periodo = *tiempo_fin - *tiempo_inicio;
      if (periodo > 0) {
        *frecuencia = 1000000.0 / periodo;
      }
      *tiempo_inicio = *tiempo_fin;  // Actualizar el tiempo de inicio
    }
  }

  // Calcular la amplitud
  float voltaje_min = valor_min * (5.0 / 1023.0);
  float voltaje_max = valor_max * (5.0 / 1023.0);
  *amplitud = (voltaje_max - voltaje_min) / 2.0;  // Amplitud en voltios pico a pico

  // Identificar la forma de onda
  identificarFormaOnda(valor_min, valor_max);
}

// Función para identificar la forma de onda
void identificarFormaOnda(int valor_min, int valor_max) {
  int cambio = valor_max - valor_min;

  if (cambio > 900) {
    strcpy(forma_onda, "Cuadrada");
  } else if (cambio > 50 && cambio <= 900) {
    strcpy(forma_onda, "Triangular");
  } else if (cambio <= 50) {
    strcpy(forma_onda, "Senoidal");
  } else {
    strcpy(forma_onda, "Desconocida");
  }
}

// Función para actualizar el LCD
void actualizarLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Freq:");
  lcd.print(*frecuencia, 2);
  lcd.print("Hz");

  lcd.setCursor(0, 1);
  lcd.print("Amp:");
  lcd.print(*amplitud, 2);
  lcd.print("V ");
  lcd.print(forma_onda);
}

// Función para mostrar los resultados en la pantalla LCD y el monitor serie
void mostrarResultados() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Procesando...");

  // Mostrar en el monitor serie
  Serial.print("Frecuencia: ");
  Serial.print(*frecuencia, 2);
  Serial.println(" Hz");

  Serial.print("Amplitud: ");
  Serial.print(*amplitud, 2);
  Serial.println(" V");

  Serial.print("Forma de onda: ");
  Serial.println(forma_onda);

  // Mostrar los datos en el LCD
  actualizarLCD();
}

// Función para liberar la memoria dinámica
void liberarMemoria() {
  free(buffer);
  free(frecuencia);
  free(amplitud);
  free(forma_onda);
  free(tiempo_inicio);
  free(tiempo_fin);
}