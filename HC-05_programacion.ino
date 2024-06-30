#include <SoftwareSerial.h>

// Define los pines para SoftwareSerial
SoftwareSerial mySerial(2, 3);

char valor;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);  // Comunicación con el Arduino Uno
}

void loop() {
  if (Serial.available()) {
    char received = Serial.read();
    Serial.print("Este es Nano: ");
    Serial.println(received);
    mySerial.write(received);  // Enviar datos al Arduino Uno
  }
}
