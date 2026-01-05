---
title: "ESP32-DIV"
description: "dwufunkcyjny zestaw narzędzi do testowania sieci bezprzewodowych"
youtubeId: "4AfNkUe3eHo"
date: 2025-01-31
tags: ["Tutorial", "ESP32", "DIV", "CiferTech"]
relatedProject: "div"
---

## O filmie

Przeznaczenie: Zestaw jest przeznaczony do testowania sieci bezprzewodowych, analizy sygnału, tworzenia zagłuszaczy i podszywania się pod protokoły. Jest przeznaczony do celów edukacyjnych i badawczych, w szczególności dla administratorów sieci, entuzjastów bezpieczeństwa i programistów IoT.
    Sprzęt podstawowy:
    ESP32-U (16 MB): Główny mikrokontroler zapewniający obsługę Wi-Fi i Bluetooth.
    Wyświetlacz TFT LCD ST7735 lub ILI9341: Do interfejsu użytkownika i wizualizacji danych w czasie rzeczywistym.
    Wiele modułów nRF24: Umożliwia obsługę pasma 2,4 GHz, takich jak skanowanie, zagłuszanie i analiza protokołów.
    Moduł CC1101: Do zagłuszania w paśmie sub-GHz, ataków typu replay i przechwytywania sygnałów.
    Inne komponenty: W tym regulator napięcia, ładowarka baterii, konwerter USB-na-szeregowy, ekspander I/O, gniazdo kart SD i przyciski nawigacyjne.

## Omawiane tematy:

  -    Monitor pakietów: Wyświetlanie pakietów Wi-Fi w czasie rzeczywistym, często wizualizowane jako wykres kaskadowy dla różnych kanałów.
  -    Skaner Wi-Fi: Wykrywa i wyświetla listę pobliskich sieci Wi-Fi, pokazując identyfikatory SSID, siłę sygnału, kanały i protokoły bezpieczeństwa.
  - Beacon Spammer: Rozsyła fałszywe lub losowe identyfikatory SSID, symulując nieistniejące sieci (w celach edukacyjnych).
  -  Detektor deauthentication Detector: Monitoruje pakiety deauthentication, które mogą wskazywać na nieautoryzowane włamania i ostrzega użytkownika.
  - Atak deauthentication Atak Wi-Fi: Wysyła ramki deauthentication, aby przerwać połączenia klientów (przeznaczony do testowania odporności sieci).
  - Skaner/Spoofer BLE: Skanuje i podszywa się pod urządzenia Bluetooth Low Energy.
  - Zagłuszanie/odtwarzanie 2,4 GHz i Sub-GHz: Możliwości w wielu pasmach częstotliwości.


## Linki z filmu:

- [Link do schematu:](https://drive.google.com/drive/folders/1LC3lMyO2mR9lKcW0ZJrTwzMl_PfR1wWU)
- [Link do pliku Flash:](https://github.com/cifertech/ESP32-DIV/tree/main/Flash%20File)
- [Link do oprogramowania układowego:](https://github.com/cifertech/ESP32-DIV)