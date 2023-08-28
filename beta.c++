#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SoftwareSerial.h>

#define WATER_PUMP_PIN 8
#define RED_LED_PIN 3
#define GREEN_LED_PIN 4
#define BLUE_LED_PIN 5
#define FAN_PIN 6
#define SOIL_MOISTURE_PIN A1
#define DHT_PIN A2

#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2); 

SoftwareSerial bluetooth(12, 13);

void setup() {
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(WATER_PUMP_PIN, LOW);
  dht.begin();
  lcd.init();
  lcd.backlight();
  bluetooth.begin(9600);
}

void loop() {
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  if (soilMoisture < 500) {
    digitalWrite(WATER_PUMP_PIN, HIGH);
  } else {
    digitalWrite(WATER_PUMP_PIN, LOW);
  }
  int hour = hour();
  if (hour >= 10 && hour < 22) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(BLUE_LED_PIN, HIGH);
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, LOW);
  }
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");
  if (bluetooth.available()) {
    char command = bluetooth.read();
    if (command == '1') {
      digitalWrite(FAN_PIN, HIGH);
    } else if (command == '0') {
      digitalWrite(FAN_PIN, LOW);
    }
  }
  delay(1000);
}