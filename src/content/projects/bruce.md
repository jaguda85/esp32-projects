---
title: "ESP32 BRUCE PREDATORY"
description: "Do testów sieci wifi, bluetooth i nie tylko"
mainImage: "/esp32-projects/images/projects/bruce/main.jpg"
images:
  - "/esp32-projects/images/projects/bruce/photo1.jpg"
  - "/esp32-projects/images/projects/bruce/photo2.jpg"
  - "/esp32-projects/images/projects/bruce/photo3.jpg"
  - "/esp32-projects/images/projects/bruce/photo4.jpg"
  - "/esp32-projects/images/projects/bruce/photo5.jpg"
  - "/esp32-projects/images/projects/bruce/photo6.jpg"
  - "/esp32-projects/images/projects/bruce/photo7.jpg"
  - "/esp32-projects/images/projects/bruce/photo8.jpg"
date: 2025-01-20
tags: ["ESP32", "wifi", "bluetooth"]
difficulty: "Trudny"
components:
  - "ESP32 DevKit v1"
  - "2.4' lub 2.8' TFT SPI ILI9341 LCD"
  - "NRF24L01 PA LNA 2.4Ghz"
  - "RF TRANSCEIVER CC1101"
  - "RFID PN532"
  - "GPS Ublo NEO-6M"
  - "IR TX KY-005 i RX 38khz TSOP38438"
  - "DIP Switch 2 Pin 2 Channel"
  - "MICRO SD MODULE"
  - "2 Rezystory 10 K Ohm"
  - "2 Kondensatory 10uf"
  - "2 Kondensatory 100nf"

#Dostępne piny po lewej:
#EN, 36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, GND, 13, 9, 10, 11, 6, 7, 8

#Dostępne piny po prawej:
#3V3, GND, 15, 2, 0, 4, 16, 17, 5, 18, 19, 21, RX, TX, 22, 23, GND, 3V3, 5V, CMD
# PINY - LEWA STRONA
leftPins:
 -pin: "3V3"
connection: ""
-pin: "EN"
connection: "TFT_RST"
-pin: "36"
connection: "T_IRQ"
-pin: "39"
connection: "T_DO"
-pin: "32"
connection: "T_DIN"
-pin: "27"
connection: "NRF24_CSN; PN532_SDA; IR_TX"
-pin: "13"
connection: "TFT_MOSI"
-pin: "12"
connection: "TFT_MISO"
-pin: "34"
connection: ""
-pin: "35"
connection: ""
-pin: "33"
connection: "T_CS"
-pin: "25"
connection: "T_CLK"
-pin: "26"
connection: ""
-pin: "14"
connection: "TFT_SCK"

# PINY - PRAWA STRONA
rightPins:
  - pin: "GND"
    connection: "Masa wspólna"
  - pin: "23"
    connection: "NRF24_MOSI; SD_MOSI"
  - pin: "15"
    connection: "TFT_CS"
  - pin: "2"
    connection: "TFT_DC/RS"
  - pin: "4"
    connection: ""
  - pin: "5"
    connection: "SD_CS"
  - pin: "18"
    connection: "NRF24_SCK; SD_SCK"
  - pin: "19"
    connection: "NRF24_MISO; SD_MISO; "
  - pin: "21"
    connection: "TFT_LED"
  - pin: "TX"
    connection: "GPS_TX"
  - pin: "RX"
    connection: "GPS_RX"
  - pin: "22"
    connection: "HRF24_CE; PN532_SCL; IR_RX"
  - pin: "3V3"
    connection: "Zasilanie czujnika"
---

## Opis projektu

Bruce ma być wszechstronnym oprogramowaniem ESP32, które obsługuje mnóstwo ofensywnych funkcji skupiających się na ułatwianiu operacji Red Team. Obsługuje także produkty M5stack i Lilygo i świetnie współpracuje z Cardputerami, Stickami, M5Cores, T-Decks i T-Embeds.

## Jak to działa

Wyjaśnij działanie...

## Kod programu
Pobierz plik:   https://github.com/BruceDevices/firmware/releases/tag/1.13

## Link do Github:  https://github.com/BruceDevices/firmware

## Podsumowanie

Co osiągnąłeś...