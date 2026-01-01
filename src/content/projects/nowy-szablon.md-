---
title: "Nazwa projektu"
description: "Krótki opis projektu"
mainImage: "/esp32-projects/images/projects/nazwa/main.jpg"
date: 2025-01-20
tags: ["ESP32", "Tag2"]
difficulty: "Średni"
components:
  - "ESP32 DevKit v1"
  - "Komponent 1"

## Kod programu
Pobierz plik: [waga.ino](/esp32-projects/code/??/??.ino)

arduinoFilename: "nazwa_projektu.ino"

arduinoCode: |
  /*
   * Nazwa projektu
   * Autor: Twoje Imię
   * Data: 2025-01-20
   */
  
  #define LED_PIN 25
  #define BUTTON_PIN 39
  
  void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Serial.begin(115200);
    Serial.println("Program uruchomiony!");
  }
  
  void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON");
    } else {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF");
    }
    delay(100);
  }

leftPins:
  - pin: "25"
    connection: "LED + rezystor 220Ω"
  - pin: "39"
    connection: "Przycisk"

rightPins:
  - pin: "GND"
    connection: "Masa wspólna"
  - pin: "3V3"
    connection: "Zasilanie przycisku"
---

## Opis projektu

Szczegółowy opis...

## Jak to działa

Wyjaśnienie działania...

## Możliwe rozszerzenia

- Dodaj więcej LED
- Użyj PWM do płynnego migania