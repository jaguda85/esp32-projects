---
title: "ESP GRABBER"
description: "Odbiera i wysyła sygnały pilotów"
mainImage: "/esp32-projects/images/projects/espgrabber/main.png"
images:
  - "/esp32-projects/images/projects/espgeabber/photo2.png"
  - "/esp32-projects/images/projects/espgrabber/photo1.png"
date: 2025-01-20
tags: ["ESP32", "Tag2"]
difficulty: "Średni"
boardType: "esp32"
components:
  - "ESP32"
  - "EKRAN OLED 0.96 I2C"
  - "CC1101 RF transceiver"
  - "4 przyciski"

# PINY - LEWA STRONA (od góry: 3V3, EN, 36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, GND, 13, 3, CMD, 5V)
leftPins:
  - pin: "3V3"
    connection: "Zasilanie 3.3V"
  - pin: "32"
    connection: "przycisk UP"
  - pin: "33"
    connection: "PRZYCISK DPWN"
  - pin: "26"
    connection: "PRZYCISK OK"
  - pin: "27"
    connection: "PRZYCISK BACK"
  - pin: "GND"
    connection: "Masa"

# PINY - PRAWA STRONA (od góry: GND, 23, 22, TX, RX, 21, GND, 19, 18, 5, 17, 16, 4, 0, 2, 15, SD0, SD1, CLK)
rightPins:
  - pin: "GND"
    connection: "Masa wspólna"
  - pin: "23"
    connection: "CC1101 6"
  - pin: "22"
    connection: "OLED SCK"
  - pin: "21"
    connection: "OLED SDA"
  - pin: "RX"
    connection: ""
  - pin: "19"
    connection: "CC1101 7"
  - pin: "18"
    connection: "CC1101 5"
  - pin: "5"
    connection: "CC1101 4"
  - pin: "2"
    connection: "CC1101 3"
---

## Opis projektu

ESP-GRABER to wszechstronne narzędzie do pracy z częstotliwościami radiowymi w pasmach 315 / 433 / 868 / 915 MHz, oparte na mikrokontrolerze ESP32 z modułem RF CC1101 i wyświetlaczem OLED.
Oprogramowanie sprzętowe umożliwia odczytywanie, powtarzanie, analizowanie, przechowywanie i — przy zachowaniu ostrożności — zakłócanie sygnałów RF. Jest przeznaczony wyłącznie do celów edukacyjnych i testowych.