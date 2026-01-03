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

Projekt zegara z datą i pogodą. Posiada AP z www gdzie można dokonać zmiany wielu parametrów. Wymaga połącznia wifi i API na 'openweather.com'. 
Koniecznie uzupełnij je w kodzie.

## Schemat połączeń

# PINY - PRAWA STRONA
rightpins:
  - pin: "18"
    connection: "CLK"
  - pin: "23"
    connection: "DATA_PIN"
  - pin: "5"
    connection: "CS_PIN"
  

## Jak to działa
Znajdź na początku kodu i uzupełnij poniższe parametry:
// Konfiguracja WiFi - Access Point
const char* ap_ssid = "TU WPISZ NAZWĘ AP";
const char* ap_password = "TU WPISZ HASŁO DO AP(CO NAJ MNIEJ 6 ZNAKÓW)";

// Konfiguracja WiFi - połączenie z internetem
const char* sta_ssid = "TU WPISZ NAZWĘ TWOJEGO WIFI";
const char* sta_password = "TU HASŁO DO TWOJEGO WIFI";

// Konfiguracja OpenWeatherMap
const char* weatherApiKey = "TU WPISZ KOD API Z OPENWEATHER.COM";
const char* weatherCity = "TU WPISZ SWOJE MIASTO";
const char* weatherCountry = "PL";

API założysz za darmo na stronie openweather.com



## Kod programu

Pobierz plik: [zegarmax.ino](/esp32-projects/code/zegarmax/zegarmax.ino)



## Możliwe rozszerzenia

