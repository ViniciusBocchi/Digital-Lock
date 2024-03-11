#define BLYNK_AUTH_TOKEN "zIpGYMRiWda5ruUMqhxwbAp9D5kwp2TG"
#define BLYNK_TEMPLATE_ID "TMPL2dA6Ahd3s"
#define BLYNK_TEMPLATE_NAME "ESP APP"

#define APP 15
#include <SPI.h>
#include <BlynkSimpleEsp32.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>

#define RST_PIN 22
#define SS_PIN 5
#define RED_LED_PIN 4
#define GREEN_LED_PIN 2
#define SERVO_PIN 15
#define BLYNK_PRINT Serial

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo;

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "iPhone";
char pass[] = "loko123123";

bool servoOpen = false; // Flag para verificar se o servo está aberto
unsigned long servoOpenTime = 0; // Variável para armazenar o tempo em que o servo foi aberto

BLYNK_WRITE(V15){
bool value1 = param.asInt(); 
if (value1 == 1) {
  digitalWrite(APP, LOW);
  openServo();
  digitalWrite(GREEN_LED_PIN, HIGH); }
else {
  digitalWrite(APP, HIGH);
  closeServo();
  digitalWrite(GREEN_LED_PIN, LOW); } 
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Configurando Wi-Fi...");
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);
  myServo.detach();
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Serial.println("Configuração Wi-Fi concluída.");
}



void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Obter o UID como uma string
    String uidString = getUidAsString();

    // Aqui você verifica se o UID do cartão é válido
    if (isCardValid(uidString)) {
      if (!servoOpen) {
        openServo();
        digitalWrite(GREEN_LED_PIN, HIGH);
        servoOpen = true;
        servoOpenTime = millis(); // Armazena o tempo atual
      }
    }

    Serial.println("UID: " + uidString);
    mfrc522.PICC_HaltA();
    delay(1000);
  }

  // Verifica se o servo está aberto por mais de 5 segundos
  if (servoOpen && millis() - servoOpenTime > 5000) {
    closeServo();
    digitalWrite(GREEN_LED_PIN, LOW);
    servoOpen = false;
  }
  Blynk.run();
}

String getUidAsString() {
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidString += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  return uidString;
}

bool isCardValid(String uidString) {
  // Substitua pela sua string UID desejada
  return (uidString == "b190161d");
}

void openServo() {
  digitalWrite(RED_LED_PIN, LOW); // Apaga o LED vermelho
  myServo.attach(15);
  myServo.write(0);
  delay(100);
  myServo.detach();
}

void closeServo() {
  digitalWrite(RED_LED_PIN, HIGH); // Mantém o LED vermelho sempre aceso
  myServo.attach(15);
  myServo.write(180);
  delay(100);
  myServo.detach();
}