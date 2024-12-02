#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Configuración de sensores y actuadores
#define DHTPIN 2          // Pin del DHT22
#define DHTTYPE DHT22     // Tipo de sensor DHT
#define LDRPIN 12         // Pin del LDR
#define PIRPIN 3          // Pin del sensor PIR
#define TRIGPIN 4         // Pin TRIG del HC-SR04
#define ECHOPIN 5         // Pin ECHO del HC-SR04
#define LEDPIN 9          // Pin del LED
#define SERVOPIN 6        // Pin del Servo
#define BUZZERPIN 10      // Pin del Buzzer
#define RELAYPIN 11       // Pin del Relay (ventilador)

DHT dht(DHTPIN, DHTTYPE);         // Objeto para manejar el sensor DHT22
LiquidCrystal_I2C lcd(0X27, 16, 2); // LCD I2C, dirección 0x27
Servo myServo;                    // Objeto para manejar el servo

void setup() {
  // Configuración de pines
  pinMode(LDRPIN, INPUT);
  pinMode(PIRPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(RELAYPIN, OUTPUT);

  // Inicialización de componentes
  dht.begin();
  myServo.attach(SERVOPIN);
  myServo.write(0); // Inicializar servo en posición 0
  
  lcd.begin(16, 2); // Inicializa LCD de 16x2
  lcd.backlight();  // Enciende la retroiluminación de la pantalla LCD

  lcd.setCursor(0, 0);
  lcd.print("Iniciando..."); // Mensaje de inicio
  delay(2000); // Espera para visualizar el mensaje

  lcd.clear(); // Limpia el LCD para nuevas lecturas
  
  Serial.begin(9600); // Para monitoreo en el Serial Monitor (opcional)
}

void loop() {
  // **1. Lectura del DHT22 (Temperatura y Humedad)**
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // **2. Lectura del LDR (Nivel de luz)**
  int darkLevel = digitalRead(LDRPIN);

  // **3. Lectura del PIR (Movimiento detectado)**
  int motionDetected = digitalRead(PIRPIN);

  // **4. Lectura del HC-SR04 (Distancia en cm)**
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  int duration = pulseIn(ECHOPIN, HIGH);
  float distance = duration * 0.034 / 2; // Convertir a cm

  // **5. Mostrar datos en el LCD**
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temp, 1); // Mostrar temperatura con 1 decimal
  lcd.print("C ");

  lcd.setCursor(0, 1);
  lcd.print("Dist:");
  lcd.print(distance, 1); // Mostrar distancia con 1 decimal
  lcd.print("cm ");

  // **6. Control de actuadores según las lecturas**

  // Control del LED y Relay basado en temperatura o luminosidad
  if (temp > 30 || darkLevel == HIGH) {
    digitalWrite(LEDPIN, HIGH);  // Enciende LED
    digitalWrite(RELAYPIN, HIGH); // Activa relay (simula ventilador o luz externa)
  } else {
    digitalWrite(LEDPIN, LOW);   // Apaga LED
    digitalWrite(RELAYPIN, LOW); // Desactiva relay
  }

  // Control del buzzer y servo basado en movimiento detectado
  if (motionDetected) {
    digitalWrite(BUZZERPIN, HIGH); // Activa buzzer
    myServo.write(90);             // Mueve el servo a 90 grados
  } else {
    digitalWrite(BUZZERPIN, LOW);  // Apaga buzzer
    myServo.write(0);              // Retorna servo a posición inicial
  }

  // Alerta en LCD si distancia es menor a 10 cm
  if (distance < 10) {
    lcd.setCursor(0, 1);
    lcd.print("Alerta Prox!   ");
    digitalWrite(BUZZERPIN, HIGH); // Activa buzzer
  } else {
    digitalWrite(BUZZERPIN, LOW);  // Apaga buzzer si distancia segura
  }

  delay(1000); // Retardo de 1 segundo
}

