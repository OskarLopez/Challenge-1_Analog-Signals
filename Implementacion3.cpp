// Declaración de pines para los pulsadores
const int buttonPin1 = 2;  // Pulsador 1 en pin 2
const int buttonPin2 = 3;  // Pulsador 2 en pin 3

// Variables para manejar el estado de los pulsadores
bool *buttonState;  // Puntero para almacenar el estado de los pulsadores
unsigned long *lastDebounceTime;  // Puntero para almacenar el tiempo de antirebote
unsigned long debounceDelay = 50;  // Tiempo de antirebote (50 ms)

// Variables para almacenar el estado anterior de los pulsadores
bool lastButtonState1 = LOW;
bool lastButtonState2 = LOW;

void setup() {
  // Inicialización de pines de entrada para los pulsadores
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);

  // Inicializar el monitor serie
  Serial.begin(9600);

  // Asignación dinámica de memoria para el estado de los pulsadores
  buttonState = (bool *)malloc(2 * sizeof(bool));  // Almacenar el estado de dos pulsadores
  lastDebounceTime = (unsigned long *)malloc(2 * sizeof(unsigned long));  // Almacenar el tiempo para dos pulsadores

  // Inicializar los tiempos de antirebote en 0
  lastDebounceTime[0] = 0;
  lastDebounceTime[1] = 0;
}

void loop() {
  // Leer el estado de los pulsadores con antirebote
  buttonState[0] = leerAntirebote(buttonPin1, 0, lastButtonState1);
  buttonState[1] = leerAntirebote(buttonPin2, 1, lastButtonState2);

  // Verificar si los pulsadores han sido presionados
  if (buttonState[0]) {
    Serial.println("Pulsador 1 presionado");
  }
  if (buttonState[1]) {
    Serial.println("Pulsador 2 presionado");
  }

  delay(10);  // Pequeño retardo para evitar saturar el monitor serie
}

// Función para leer el estado del pulsador con antirebote
bool leerAntirebote(int pin, int index, bool &lastButtonState) {
  bool reading = digitalRead(pin);  // Leer el estado del pulsador

  // Comprobar si el estado ha cambiado (posible rebote)
  if (reading != lastButtonState) {
    lastDebounceTime[index] = millis();  // Reiniciar el temporizador de antirebote
  }

  // Si ha pasado suficiente tiempo (antirebote)
  if ((millis() - lastDebounceTime[index]) > debounceDelay) {
    // Verificar si el estado del pulsador ha cambiado realmente
    if (reading != buttonState[index]) {
      buttonState[index] = reading;

      // Solo devolver "true" si el pulsador está presionado
      if (buttonState[index] == HIGH) {
        lastButtonState = reading;  // Actualizar el estado anterior
        return true;
      }
    }
  }

  // Actualizar el estado anterior
  lastButtonState = reading;

  // Devolver false si el pulsador no ha sido presionado
  return false;
}
