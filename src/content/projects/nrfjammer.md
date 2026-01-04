---
title: "Cypher jammer mini"
description: "Kompaktowy projekt IoT"
mainImage: "/esp32-projects/images/projects/nrfjammer/main.jpg"
date: 2025-01-20
tags: ["ESP32-C3", "Compact"]
difficulty: "Łatwy"
boardType: "c3supermini"
components:
  - "ESP32-C3 Super Mini"
  - "2x Nrf"
  - "3x przyciski"
  - "Ekran OLED 1.3' SSD1306"
# PINY - LEWA STRONA (od góry: 5, 6, 7, 8, 9, 10, 20, 21)
leftPins:
  - pin: "5"
    connection: "LED Data"
  - pin: "6"
    connection: "Sensor"
  - pin: "20"
    connection: "I2C SDA"
  - pin: "21"
    connection: "I2C SCL"

# PINY - PRAWA STRONA (od góry: 5V, GND, 3V3, 4, 3, 2, 1, 0)
rightPins:
  - pin: "5V"
    connection: "Zasilanie (USB)"
  - pin: "GND"
    connection: "Masa"
  - pin: "0"
    connection: "Boot/LED"
  - pin: "1"
    connection: "UART TX"
  - pin: "2"
    connection: "UART RX"
---

## Opis projektu

Szczegółowy opis...
