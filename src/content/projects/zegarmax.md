---
title: "ZEGAR MAX"
description: "Zegar z wyświetlaczem MAX7219"
mainImage: "/esp32-projects/images/projects/zegarmax/main.jpg"
images:
  - "/esp32-projects/images/projects/zegarmax/photo1.jpg"
  - "/esp32-projects/images/projects/zegarmax/photo2.jpg"
date: 2024-01-15
tags: ["LED", "Podstawy", "GPIO"]
components:
  - "ESP32"
  - "Dioda LED"
  - "Rezystor 220Ω"
  - "Przewody połączeniowe"
difficulty: "Łatwy"
---

## Opis projektu

To jest mój pierwszy projekt z ESP32! Dioda LED miga co sekundę.

## Schemat połączeń

- LED anoda → GPIO 2 (przez rezystor 220Ω)
- LED katoda → GND

## Jak to działa

Program używa funkcji `digitalWrite()` do włączania i wyłączania diody LED.
Funkcja `delay()` dodaje pauzę między zmianami stanu.

## Kod programu

Pobierz plik: [zegarmax.ino](/code/zegarmax/zegarmax.ino)

```cpp
// Migająca dioda LED na ESP32
#define LED_PIN 2

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Program started!");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);  // Włącz LED
  Serial.println("LED ON");
  delay(1000);                  // Czekaj 1 sekundę
  
  digitalWrite(LED_PIN, LOW);   // Wyłącz LED
  Serial.println("LED OFF");
  delay(1000);                  // Czekaj 1 sekundę
}
```

## Możliwe rozszerzenia

- Dodaj przycisk do zmiany częstotliwości migania
- Użyj PWM do płynnego zapalania/gaśnięcia
- Dodaj więcej diod LED w różnych kolorach