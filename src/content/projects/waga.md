---
title: "waga"
description: "Waga z OLED do 1kg"
mainImage: "/esp32-projects/images/projects/waga/main.jpg"
images:
  - "/esp32-projects/images/projects/waga/photo1.jpg"
  - "/esp32-projects/images/projects/waga/photo2.jpg"
date: 2024-01-20
tags: ["Tag1", "Tag2", "Tag3"]
components:
  - "ESP32"
  - "HX711"
  - "2 przyciski"
  - "OLED 1.3' SH1106 I2C"
difficulty: "Łatwy"  # lub "Średni" lub "Trudny"

# PINY
rightPins:
  - pin: "22"
    connection: "SDA OLED"
  - pin: "21"
    connection: "SCL OLED"
  - pin: "4"
    connection: "DOUT HX711"
  - pin: "5"
    connection: "SCL HX711"

leftPins:
  - pin: "26"
    connection: "BUZZER +PIN"
---
## Opis projektu

Szczegółowy opis projektu...

## Schemat połączeń

- Pin 22 → OLED SDA
- Pin 21 → OLED SCL
- Pin 26 → Buzzer aktywny
- Pin 4 → HX711 DOUT
- Pin 5 → HX711 SCL

## Kod programu
Pobierz plik: [waga.ino](/esp32-projects/code/waga/waga.ino)
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
