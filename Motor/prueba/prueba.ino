// pines para el motor
#define dirPin 2
#define stepPin 3

// REVOLUCIONES DEL MOTOR 172 = 1CM
#define stepsPerRevolution 198 // Número de pasos por revolución para el motor

// BOTONES ADELANTE
#define adelante 4
// LED RGB
const int redPin = 5;
const int greenPin = 6;
const int bluePin = 7;

// BOTON ACEPTAR
#define aceptar 8
int aceptarAzul = 9;
int aceptarRojo = 10;

// BOTON ATRAS
#define atras 11
int atrasAzul = 13;
int atrasRojo = 12;

// PANTALLA LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inicializamos la pantalla LCD con la dirección 0x27 y tamaño 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables globales para la distancia y el potenciómetro
int distancia = 0; 
const int potPin = A1;
int distanciaObjetivo = 0;
int lastDistancia = -1;
int lastDistanciaObjetivo = -1;

// BLUETOOTH VARIABLE RECIBIDA
char received = '\0';
char receivedDigits[4] = {'\0', '\0', '\0', '\0'}; // Almacena los dígitos recibidos
int digitIndex = 0;

void setup() {
  // Inicializamos la pantalla LCD y la comunicación serial
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  // Configuramos los pines como entrada o salida según corresponda
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // BOTON ACEPTAR
  pinMode(aceptar, INPUT_PULLUP);
  pinMode(aceptarAzul, OUTPUT);
  pinMode(aceptarRojo, OUTPUT);
  
  //BOTON ADELANTE
  pinMode(adelante, INPUT_PULLUP); 
  // Configuración de los pines del LED RGB
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // BOTON ATRAS
  pinMode(atras, INPUT_PULLUP); 
  pinMode(atrasAzul, OUTPUT); // LED AZUL
  pinMode(atrasRojo, OUTPUT); // LED ROJO
}

void loop() {

  // COMUNICACION CON PYTHON
  char cambio = Serial.read();
  Serial.println(cambio);
  delay(1000);


  // Verificamos si hay datos disponibles en el puerto serial
  if (Serial.available()) {
    received = Serial.read();
    Serial.print("Este es Uno: ");
    Serial.println(received);

    if (isdigit(received)) {
      receivedDigits[digitIndex++] = received; // Almacena el dígito en el array
    } else if (received == ',') {
      // Si se recibe una coma, se interpreta como el final del número
      receivedDigits[digitIndex] = '\0'; // Añade terminador de string
      distanciaObjetivo = atoi(receivedDigits); // Convierte a entero
      digitIndex = 0; // Reinicia el índice para el próximo número
      Serial.print("Distancia Objetivo: ");
      Serial.println(distanciaObjetivo);

      moverMotorABluetooth();
    }

    // Limpiar el buffer de entrada serial
    while (Serial.available() > 0) {
      Serial.read();
    }
  }

  // ENCENDEMOS LOS LEDS(AZUL) 
  setColor(0, 255, 0);
  digitalWrite(aceptarAzul, HIGH);
  digitalWrite(atrasAzul, HIGH);

  // Leemos el estado de los botones
  int estado_adelante = digitalRead(adelante);
  int estado_atras = digitalRead(atras);
  int estado_aceptar = digitalRead(aceptar);

  // ADELANTE CON BLUETOOTH 
  if (received == 'I') {
    if (distancia < 25){
      digitalWrite(dirPin, LOW);
      distancia += 1;
      setColor(255, 0, 0); // Rojo
      moveMotor();
      received = '\0';
    }
  }
  // VUELVE AL COLOR AZUL
    setColor(0, 255, 0); // Rojo

  // ATRAS CON BLUETOOTH
  if (received == 'D') {
    if (distancia > 0) {
      digitalWrite(dirPin, HIGH);
      distancia -= 1;
      digitalWrite(atrasAzul, LOW); // APAGA LUZ AZUL
      digitalWrite(atrasRojo, HIGH); // ENCIENDE LUZ ROJA
      moveMotor();
      received = '\0';
    } else {
      // lcd.setCursor(0, 0);s
      // lcd.print("Invalido:");
    }
  }

  // Si el botón "adelante" está presionado
  if (estado_adelante == LOW) {
    Serial.println("Boton en pin 11 presionado");
    digitalWrite(dirPin, LOW);
    distancia += 1;
    setColor(255, 0, 0); // Rojo
    moveMotor();
  }
  // VUELVE AL COLOR AZUL
    setColor(0, 255, 0); // Rojo


  // Si el botón "atrás" está presionado
  if (estado_atras == LOW) {
    Serial.println("Boton en pin 12 presionado");
    if (distancia > 0) {
      digitalWrite(dirPin, HIGH);
      distancia -= 1;
      digitalWrite(atrasAzul, LOW); // APAGA LUZ AZUL
      digitalWrite(atrasRojo, HIGH); // ENCIENDE LUZ ROJA
      moveMotor();
    } else {
      // lcd.setCursor(0, 0);
      // lcd.print("Invalido:");
    }
  }
  digitalWrite(atrasRojo, LOW); // APAGAMOS LUZ ROJA
  digitalWrite(atrasAzul, HIGH); // ENCENDEMOS LUZ AZUL


  // Leemos el valor del potenciómetro
  int potValue = analogRead(potPin);

  // Mapeamos el valor del potenciómetro (0-1023) a la distancia (0-35 cm)
  distanciaObjetivo = map(potValue, 100, 900, 0, 35);
  if (distanciaObjetivo < 0) {
  distanciaObjetivo = 0;
  }

  // Si el botón "aceptar" está presionado
  if (estado_aceptar == LOW) {
    Serial.println("Botón aceptar presionado");
    digitalWrite(aceptarRojo, HIGH); // ENCENDEMOS LUZ ROJA
    digitalWrite(aceptarAzul, LOW); // APAGAMOS LUZ AZUL

    while (distancia != distanciaObjetivo) {
      Serial.print("Distancia actual: ");
      Serial.println(distancia);
      Serial.print("Distancia objetivo: ");
      Serial.println(distanciaObjetivo);
      if (cambio == '\xff'){
        break;
      }else{
        if (distanciaObjetivo > distancia) {
        digitalWrite(dirPin, LOW);
        moveMotor();
          distancia += 1;
        } else {
          digitalWrite(dirPin, HIGH);
          moveMotor();
          distancia -= 1;
        }
        actualizarLCD();
      }
    }
  }

  // APAGAMOS ROJO Y ENCENDEMOS AZUL
  digitalWrite(aceptarRojo, LOW);
  digitalWrite(aceptarAzul, HIGH);

  // Actualiza la LCD después de cada cambio de distancia y potenciómetro
  actualizarLCD();
}

// Función para mover el motor un número determinado de pasos
void moveMotor() {
  for (int i = 0; i < 5 * stepsPerRevolution; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(900);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(900);
  }
}

// Función para establecer el color del LED RGB
void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

// Función para actualizar el valor de la distancia en la LCD
void actualizarLCD() {
  if (distancia != lastDistancia || distanciaObjetivo != lastDistanciaObjetivo) {
    char buffer[16];
    lcd.setCursor(0, 0);
    lcd.print("Actual:");
    lcd.setCursor(8, 0);
    sprintf(buffer, "%02d cm", distancia);
    lcd.print(buffer);
  
    lcd.setCursor(0, 1);
    lcd.print("Obj:");
    lcd.setCursor(8, 1);
    sprintf(buffer, "%02d cm", distanciaObjetivo);
    lcd.print(buffer);

    // Actualiza los últimos valores registrados
    lastDistancia = distancia;
    lastDistanciaObjetivo = distanciaObjetivo;
  }
}

void moverMotorABluetooth() {
  while (distancia != distanciaObjetivo) {
    digitalWrite(aceptarRojo, HIGH); // ENCENDEMOS LUZ ROJA
    digitalWrite(aceptarAzul, LOW); // APAGAMOS LUZ AZUL
    Serial.print("Distancia actual: ");
    Serial.println(distancia);
    Serial.print("Distancia objetivo: ");
    Serial.println(distanciaObjetivo);
    if (distanciaObjetivo > distancia) {
      digitalWrite(dirPin, LOW);
      moveMotor();
      distancia += 1;
    } else {
      digitalWrite(dirPin, HIGH);
      moveMotor();
      distancia -= 1;
    }
    actualizarLCD();
  }
  // APAGAMOS ROJO Y ENCENDEMOS AZUL
  digitalWrite(aceptarRojo, LOW);
  digitalWrite(aceptarAzul, HIGH);
}