---
title: "Lotto-wyniki"
description: "Pokazuje wyniki losowań LOTTO"
mainImage: "/esp32-projects/images/projects/Wyniki-lotto/lotto1.jpg"
images:
  - "/esp32-projects/images/projects/Wyniki-lotto/lotto2.jpg"
  - "/esp32-projects/images/projects/Wyniki-lotto/lotto3.jpg"
  - "/esp32-projects/images/projects/Wyniki-lotto/lotto4.jpg"
date: 2026-07-12
tags: ["ESP32", "OpenAPI", "Lotto"]
difficulty: "Średni"
components:
  - "ESP32 DevKit v1"
  - "TFT 2.8'' ILI9341"
  - "6 przycisków"
  - "Przewody"
libraries:
  - name: "TFT_eSPI"
    author: ""
leftPins:
  - pin: "3V3"
    connection: "Zasilanie LED"
  - pin: "25"
    connection: "PRZYCISK WSTECZ"
  - pin: "26"
    connection: "PRZYCISK OK"
  - pin: "27"
    connection: "PRZYCISK DÓL"
  - pin: "12"
    connection: "PRZYCISK LEWO"
  - pin: "13"
    connection: "PRZYCISK PRAWO"
  - pin: "14"
    connection: "PRZYCISK GÓRA"
  - pin: "GND"
    connection: "Masa"
rightPins:
  - pin: "GND"
    connection: "Masa wspólna"
  - pin: "23"
    connection: "TFT_MOSI"
  - pin: "19"
    connection: "TFT_MISO"
  - pin: "18"
    connection: "TFT_SCLK"
  - pin: "4"
    connection: "TFT_RST"
  - pin: "2"
    connection: "TFT_DC"
  - pin: "15"
    connection: "TFT_CS"
arduinoFilename: "lotto_TRY_05a.ino"

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
