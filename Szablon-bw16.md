---
title: "Projekt na BW16"
description: "Projekt WiFi/BLE"
mainImage: "/images/projects/nazwa/main.jpg"
date: 2025-01-20
tags: ["BW16", "RTL8720DN", "WiFi"]
difficulty: "Zaawansowany"
boardType: "bw16"
components:
  - "BW16 RTL8720DN"
  - "Czujnik I2C"

# PINY - LEWA STRONA (od góry: GND, PA30, PA27, PA25, PA26, PA8, PA7, EN, GND, 3V3, 5V)
leftPins:
  - pin: "PA30"
    connection: "I2C SDA"
  - pin: "PA27"
    connection: "I2C SCL"
  - pin: "PA26"
    connection: "LED"
  - pin: "GND"
    connection: "Masa"
  - pin: "3V3"
    connection: "Zasilanie czujnika"

# PINY - PRAWA STRONA (od góry: GND, PA15, PA14, PA13, PA12, PA3, PA2, PA1, GND, 3V3, 5V)
rightPins:
  - pin: "PA15"
    connection: "Button"
  - pin: "PA14"
    connection: "UART TX"
  - pin: "PA13"
    connection: "UART RX"
  - pin: "GND"
    connection: "Masa wspólna"
---

## Opis projektu

Szczegółowy opis...
