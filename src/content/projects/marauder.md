---
title: "MARAUDER ESP32"
description: "Zabawa z wifi i BT"
mainImage: "/esp32-projects/images/projects/marauder/main.jpg"
images: 
-  "/esp32-projects/images/projects/marauder/photo1.jpg"
-  "/esp32-projects/images/projects/marauder/photo2.jpg"
date: 2025-01-20
tags: ["ESP32", "wifi"]
difficulty: "Średni"
components:
  - "ESP32 DevKit v1"
  - "Komponent 1"
  - "Komponent 2"

#Dostępne piny po lewej:
#EN, 36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, GND, 13, 9, 10, 11, 6, 7, 8

#Dostępne piny po prawej:
#3V3, GND, 15, 2, 0, 4, 16, 17, 5, 18, 19, 21, RX, TX, 22, 23, GND, 3V3, 5V, CMD
# PINY - LEWA STRONA
leftPins:
  - pin: "36"
    connection: "T_IRQ"
  - pin: "32"
    connection: "LED"
  - pin: "34"
    connection: ""
  - pin: "25"
    connection: ""
  - pin: "26"
    connection: "Buzzer"
  - pin: "27"
    connection: "Buzzer"

# PINY - PRAWA STRONA
rightPins:
  - pin: "GND"
    connection: "Masa wspólna"
  - pin: "23"
    connection: "TFT_MOSI; T_DIN"
  - pin: "15"
    connection: "TFT_CS"
  - pin: "2"
    connection: "TFT_DC/RS"
  - pin: "4"
    connection: "RESET"
  - pin: "5"
    connection: ""
  - pin: "18"
    connection: "TFT_SCK; T_CLK"
  - pin: "19"
    connection: "TFT_MISO; T_DO"
  - pin: "21"
    connection: "T_CS"
  - pin: "22"
    connection: "I2C SCL"
  - pin: "3V3"
    connection: "Zasilanie czujnika"
---

## Opis projektu

ESP32 Marauder to zestaw narzędzi ofensywnych i defensywnych WiFi/Bluetooth stworzony dla ESP32 i został pierwotnie zainspirowany projektem esp8266_deauther firmy Spacehuhn. Samo narzędzie pełni funkcję przenośnego urządzenia służącego do testowania i analizowania urządzeń WiFi i Bluetooth. Używaj tego narzędzia i jego oprogramowania sprzętowego ostrożnie, ponieważ korzystanie z niektórych jego możliwości bez wyraźnej zgody docelowego właściciela jest niezgodne z prawem w większości krajów.

## Jak to działa

Wyjaśnij działanie...

## Kod programu
Pobierz plik: 
```cpp
#define LED_PIN 25
#define BUTTON_PIN 39

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  delay(10);
}
```

## Podsumowanie

Co osiągnąłeś...
