#include <SoftwareSerial.h>

// Define los pines para SoftwareSerial
SoftwareSerial mySerial(2, 3);

// Definir pines
const int ledPin = 4;
const int statePin = 5;  // Pin conectado al pin STATE del HC-05

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);  // Comunicación con el módulo HC-05

  // Configuración de los pines
  pinMode(ledPin, OUTPUT);
  pinMode(statePin, INPUT);

  // Mensaje de inicio
  Serial.println("Iniciando...");
}

void loop() {
  // Leer el estado del pin STATE
  int state = digitalRead(statePin);
  
  // Imprimir el estado del pin STATE para depuración
  Serial.print("Estado del HC-05: ");
  Serial.println(state);

  // Encender o apagar el LED basado en el estado del HC-05
  if (state == HIGH) {
    digitalWrite(ledPin, HIGH);  // Bluetooth conectado
    Serial.println("LED encendido (Bluetooth conectado)");
  } else {
    digitalWrite(ledPin, LOW);   // Bluetooth en modo búsqueda
    delay(50);
    digitalWrite(ledPin, HIGH);   // Bluetooth en modo búsqueda
    delay(100);
    digitalWrite(ledPin, LOW);   // Bluetooth en modo búsqueda
    Serial.println("LED parpadea (Bluetooth en modo búsqueda)");
  }

  // Comunicación con el HC-05
  if (Serial.available()) {
    char received = Serial.read();
    Serial.print("Este es Nano: ");
    Serial.println(received);
    mySerial.write(received);  // Enviar datos al módulo HC-05
  }

  if (mySerial.available()) {
    char received = mySerial.read();
    Serial.print("HC-05 dice: ");
    Serial.println(received);
  }

  delay(1000); // Añadir un pequeño retraso para evitar sobrecarga de mensajes
}
