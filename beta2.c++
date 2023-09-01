#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define WATER_PUMP_PIN 8
#define FAN_PIN 6
#define SOIL_MOISTURE_PIN A1
#define DHT_PIN A2
#define LED_PIN D0  // WS2812B 데이터 핀
#define LED_COUNT 30 // LED 스트립에 있는 LED 수
#define RED_COLOR 255, 0, 0 // 빨간색 설정 (R, G, B)
#define ON_DURATION_MS 43200000 // 12시간 (12 * 60 * 60 * 1000)
#define OFF_DURATION_MS 43200000 // 12시간 (12 * 60 * 60 * 1000)

#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2); 
SoftwareSerial bluetooth(12, 13);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
unsigned long previousMillis = 0;
bool isOn = false;

void setup() {
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  strip.begin();
  strip.show();
  dht.begin();
  lcd.init();
  lcd.backlight();
  bluetooth.begin(9600);
}

void loop() {
  // Soil Moisture Sensing and Water Pump Control
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  if (soilMoisture < 500) { 
    digitalWrite(WATER_PUMP_PIN, HIGH);
  } else {
    digitalWrite(WATER_PUMP_PIN, LOW);
  }

  // Read Temperature and Humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");

  // Bluetooth Communication
  if (bluetooth.available()) {
    char command = bluetooth.read();
    if (command == '1') {
      digitalWrite(FAN_PIN, HIGH);
    } else if (command == '0') {
      digitalWrite(FAN_PIN, LOW);
    }
  }

  unsigned long currentMillis = millis();

  if (isOn && currentMillis - previousMillis >= ON_DURATION_MS) {
    // 켜져 있을 때 12시간이 경과하면 끔
    setStripColor(strip.Color(0, 0, 0)); // 모든 LED 끄기
    isOn = false;
    previousMillis = currentMillis;
  } else if (!isOn && currentMillis - previousMillis >= OFF_DURATION_MS) {
    // 꺼져 있을 때 12시간이 경과하면 다시 켬
    setStripColor(strip.Color(RED_COLOR));
    isOn = true;
    previousMillis = currentMillis;
  }
}

// LED 스트립의 모든 LED를 지정한 색으로 설정하는 함수
void setStripColor(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}
