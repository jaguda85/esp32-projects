---
title: "Tytuł projektu"
description: "Krótki opis projektu w 1-2 zdaniach"
mainImage: "/esp32-projects/images/projects/nazwa-projektu/main.jpg"
images:
  - "/esp32-projects/images/projects/nazwa-projektu/photo1.jpg"
  - "/esp32-projects/images/projects/nazwa-projektu/photo2.jpg"
date: 2025-01-31
tags: ["ESP32", "WiFi", "LED"]
difficulty: "Średni"
components:
  - "ESP32 DevKit v1"
  - "LED RGB"
  - "Rezystor 220Ω"
  - "Przewody połączeniowe"
libraries:
  - name: "WiFi"
    author: "Arduino"
  - name: "FastLED"
    author: "Daniel Garcia"
    link: "https://github.com/FastLED/FastLED"
leftPins:
  - pin: "3V3"
    connection: "Zasilanie LED"
  - pin: "25"
    connection: "LED Data Pin"
  - pin: "GND"
    connection: "Masa"
rightPins:
  - pin: "GND"
    connection: "Masa wspólna"
  - pin: "22"
    connection: "I2C SCL"
arduinoFilename: "projekt.ino"
arduinoCode: |
  // Twój kod Arduino tutaj
  
  void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
  }
  
  void loop() {
    // główna pętla
    delay(1000);
  }
---

## Opis projektu

Tutaj szczegółowo opisz co robi projekt, jak działa i do czego służy.

## Funkcje

- Funkcja 1
- Funkcja 2
- Funkcja 3

## Schemat połączeń

Opisz jak połączyć komponenty:
- Pin 25 → LED Data Pin
- GND → LED GND
- 3.3V → LED VCC

## Jak uruchomić

1. Podłącz komponenty według schematu
2. Wgraj kod do ESP32
3. Otwórz Serial Monitor (115200 baud)

## Możliwe rozszerzenia

- Dodaj przycisk
- Połącz z WiFi
