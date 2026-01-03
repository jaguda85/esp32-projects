---
title: "TRENAŻER"
description: "Do sprawdzania i ćwiczenia refleksu"
mainImage: "/esp32-projects/images/projects/trener/main.jpg"
images:
  - "/esp32-projects/images/projects/trener/photo1.jpg"
  - "/esp32-projects/images/projects/trener/photo2.jpg"
  - "/esp32-projects/images/projects/trener/photo3.jpg"
  - "/esp32-projects/images/projects/trener/photo4.jpg"
date: 2024-01-20
tags: ["zuzel", "trenażer", "sprzęgło"]
libraries:
  - name: "Adafruit_GFX.h"
    author: "ADAFRUIT"
  - name: "Adafruit_ILI9341.h"
leftPins:
  - pin: "35"
    connection: "POT_GAZ"
  - pin: "34"
    connection: "POT_SPRZEGLO"
  - pin: "26"
    connection: "LED ZIELONY"
  - pin: "32"
    connection: "PRZYCISK GÓRA"
  - pin: "33"
    connection: "PRZYCISK DÓŁ"
  - pin: "12"
    connection: "LEWO"
  - pin: "14"
    connection: "PRAWO"
  - pin: "21"
    connection: "OK"
  - pin: "27"
    connection: "START"
  - pin: "13"
    connection: "SET"
rightPins:
  - pin: "5"
    connection: "TFT_CS"

components:
  - "ESP32"
  - "2 potencjometry suwakowe"
  - "Ekran TFT 2.8' ILI9341"
  - "Dioda LED zielona"
difficulty: "Łatwy"  # lub "Średni" lub "Trudny"
---

## Opis projektu

Szczegółowy opis projektu...

## Schemat połączeń

- Pin 1 → Komponent A
- Pin 2 → Komponent B

## Kod programu
Pobierz plik: [trener.ino](/esp32-projects/code/trener/trener.ino)
```cpp
// Twój kod .ino tutaj

void setup() {
  // setup code
}

void loop() {
  // main code
}
```

## Podsumowanie

Co nauczyłem się w tym projekcie...