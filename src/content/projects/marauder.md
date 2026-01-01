---
title: "ESP32 MARAUDER"
description: "Do testów sieci wifi, bluetooth i nie tylko"
mainImage: "/esp32-projects/images/projects/marauder/main.jpg"
images:
  - "/esp32-projects/images/projects/marauder/photo1.jpg"
  - "/esp32-projects/images/projects/marauder/photo2.jpg"
  - "/esp32-projects/images/projects/marauder/photo3.jpg"
  - "/esp32-projects/images/projects/marauder/photo4.jpg"
  - "/esp32-projects/images/projects/marauder/photo5.jpg"
date: 2025-01-20
tags: ["ESP32", "wifi", "bluetooth"]
difficulty: "Trudny"
components:
  - "ESP32 DevKit v1"
  - "2.4' lub 2.8' TFT SPI ILI9341 LCD"
  - "GPS Ublo NEO-6M"
  - "MICRO SD MODULE"
  

# PINY - LEWA STRONA
leftPins:
  - pin: "EN"
    connection: "TFT_RST"
  - pin: "36"
    connection: ""
  - pin: "39"
    connection: ""
  - pin: "32"
    connection: "TFT_LED"
  - pin: "27"
    connection: ""
  - pin: "13"
    connection: ""
  - pin: "12"
    connection: ""
  - pin: "34"
    connection: ""
  - pin: "35"
    connection: ""
  - pin: "33"
    connection: ""
  - pin: "25"
    connection: ""
  - pin: "26"
    connection: ""
  - pin: "14"
    connection: ""

# PINY - PRAWA STRONA
rightPins:
  - pin: "GND"
    connection: "Masa wspólna"
  - pin: "23"
    connection: "TFT_MOSI; T_DIN"
  - pin: "15"
    connection: ""
  - pin: "16"
    connection: "TFT_DC/RS"
  - pin: "4"
    connection: ""
  - pin: "5"
    connection: "TFT_RST"
  - pin: "17"
    connection: "TFT_CS"
  - pin: "18"
    connection: "TFT_SCKL; T_CLK"
  - pin: "19"
    connection: "TFT_MISO; T_DO"
  - pin: "21"
    connection: "T_CS"
  - pin: "TX"
    connection: ""
  - pin: "RX"
    connection: ""
  - pin: "22"
    connection: ""
  - pin: "3V3"
    connection: "Zasilanie czujnika"
---

## Opis projektu

Czasami po prostu musisz zrobić to, co musisz zrobić. Mam rację, drogie panie? ESP32 Marauder to zestaw narzędzi ofensywnych i defensywnych WiFi/Bluetooth stworzony dla ESP32 i został pierwotnie zainspirowany projektem esp8266_deauther firmy Spacehuhn. Samo narzędzie pełni funkcję przenośnego urządzenia służącego do testowania i analizowania urządzeń WiFi i Bluetooth. Używaj tego narzędzia i jego oprogramowania sprzętowego ostrożnie, ponieważ korzystanie z niektórych jego możliwości bez wyraźnej zgody docelowego właściciela jest niezgodne z prawem w większości krajów. Aby uzyskać więcej informacji na temat tego projektu i sposobu jego montażu, kliknij poniższy link wideo. Tutaj możesz śledzić funkcje i problemy. Sprawdź #esp32marauder na Instagramie.

## Funkcje

- Skanowanie i analiza sieci WiFi
- Testy bezpieczeństwa Bluetooth
- GPS tracking
- Interfejs dotykowy TFT
- Zapis danych na karcie SD

## Instalacja firmware

1. Pobierz najnowszą wersję z GitHub
2. Użyj ESP Flash Tool lub Arduino IDE
3. Skonfiguruj piny zgodnie z powyższym schematem

## Link do firmware

**Pobierz plik:** [Najnowsze wersje](https://github.com/justcallmekoko/ESP32Marauder/releases/latest)

**GitHub:** [JustCallMeKoko](https://github.com/justcallmekoko/ESP32Marauder)

## Uwagi

- Projekt zaawansowany - wymaga doświadczenia
- Używaj tylko do legalnych testów bezpieczeństwa
- Sprawdź lokalne przepisy przed użyciem

## Podsumowanie

BESP32 Marauder to narzędzie do analizy Wi-Fi i Bluetooth. Zawiera zestaw funkcji do przechwytywania ramek, wyliczania urządzeń i transmisji ramek. Ma służyć jako urządzenie przenośne, które ma zastąpić fizycznie większe narzędzia do przechwytywania ruchu i dostarczać przechwycone dane do analizy pooperacyjnej.