// ===========================================================================
//           ZEGAR I STACJA POGODOWA - ESP32 + ILI9341
//                    Wersja 1.2 - Bez polskich znaków
// ===========================================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <time.h>

// ===========================================================================
//                         KONFIGURACJA WiFi
// ===========================================================================
const char* WIFI_SSID     = "-BaKeR-2";
const char* WIFI_PASSWORD = "xocwac-4Rannu-daxmep";

// ===========================================================================
//                       KONFIGURACJA OpenWeatherMap
// ===========================================================================
const char* OWM_API_KEY   = "80c2ac35e59be25f3bf4d97c19ffa213";
const char* OWM_CITY      = "Gniezno";
const char* OWM_COUNTRY   = "PL";
const char* OWM_UNITS     = "metric";
const char* OWM_LANG      = "pl";

String weatherURL = "http://api.openweathermap.org/data/2.5/weather?q=" 
                    + String(OWM_CITY) + "," + String(OWM_COUNTRY) 
                    + "&units=" + String(OWM_UNITS) 
                    + "&lang=" + String(OWM_LANG)
                    + "&appid=" + String(OWM_API_KEY);

// ===========================================================================
//                       KONFIGURACJA CZASU (NTP)
// ===========================================================================
const char* NTP_SERVER    = "pool.ntp.org";
const long  GMT_OFFSET    = 3600;
const int   DST_OFFSET    = 3600;

// ===========================================================================
//                         DEFINICJE KOLORÓW
// ===========================================================================
#define COLOR_BACKGROUND    TFT_BLACK
#define COLOR_TEXT          TFT_WHITE
#define COLOR_TIME          TFT_CYAN
#define COLOR_DATE          TFT_YELLOW
#define COLOR_TEMP          TFT_GREEN
#define COLOR_HUMIDITY      TFT_BLUE
#define COLOR_PRESSURE      TFT_MAGENTA
#define COLOR_WIND          TFT_ORANGE
#define COLOR_CITY          TFT_WHITE
#define COLOR_SEPARATOR     TFT_DARKGREY

// ===========================================================================
//                           PINY DODATKOWE
// ===========================================================================
#define PIN_BUZZER      25
#define PIN_BUTTON_1    34
#define PIN_BUTTON_2    35
#define PIN_BUTTON_3    33
#define PIN_LED_STATUS  26
#define PIN_BACKLIGHT   32

// ===========================================================================
//                        INTERWAŁY CZASOWE
// ===========================================================================
#define WEATHER_UPDATE_INTERVAL  600000
#define DISPLAY_UPDATE_INTERVAL  1000
#define NTP_SYNC_INTERVAL       3600000

// ===========================================================================
//                      OBIEKTY I ZMIENNE GLOBALNE
// ===========================================================================
TFT_eSPI tft = TFT_eSPI();

struct WeatherData {
    float temperature;
    float tempMin;
    float tempMax;
    float feelsLike;
    int   humidity;
    int   pressure;
    float windSpeed;
    int   windDeg;
    int   clouds;
    int   visibility;
    String description;
    String icon;
    String mainCondition;
    long  sunrise;
    long  sunset;
    bool  isValid;
} weather;

unsigned long lastWeatherUpdate = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastNtpSync = 0;

bool wifiConnected = false;
bool timeSync = false;
int  backlightLevel = 255;

String lastTimeStr = "";
String lastDateStr = "";

// ===========================================================================
//                           BUDZIK
// ===========================================================================
struct AlarmSettings {
    int  hour;
    int  minute;
    bool enabled;
    bool triggered;
    int  daysOfWeek;
} myAlarm = {7, 0, false, false, 0b0011111};

// ===========================================================================
//                        DEKLARACJE FUNKCJI
// ===========================================================================
void setupWiFi();
void setupTime();
void setupDisplay();
void updateWeather();
void drawTime();
void drawDate();
void drawWeather();
void drawWeatherIcon(int x, int y, String iconCode);
void drawStatusBar();
void drawStaticUI();
void parseWeatherData(String jsonData);
String getWindDirection(int degrees);
String removePolishChars(String input);
void checkAlarm();
void triggerAlarm();
void setBacklight(int level);

// ===========================================================================
//                    USUWANIE POLSKICH ZNAKÓW
// ===========================================================================
String removePolishChars(String input) {
    String output = "";
    
    for (int i = 0; i < input.length(); i++) {
        unsigned char c = input[i];
        
        // Sprawdź czy to znak UTF-8 (2 bajty dla polskich znaków)
        if (c == 0xC4) {
            i++;
            if (i < input.length()) {
                unsigned char c2 = input[i];
                switch (c2) {
                    case 0x85: output += 'a'; break;  // ą
                    case 0x84: output += 'A'; break;  // Ą
                    case 0x87: output += 'c'; break;  // ć
                    case 0x86: output += 'C'; break;  // Ć
                    case 0x99: output += 'e'; break;  // ę
                    case 0x98: output += 'E'; break;  // Ę
                    default: output += '?'; break;
                }
            }
        }
        else if (c == 0xC5) {
            i++;
            if (i < input.length()) {
                unsigned char c2 = input[i];
                switch (c2) {
                    case 0x82: output += 'l'; break;  // ł
                    case 0x81: output += 'L'; break;  // Ł
                    case 0x84: output += 'n'; break;  // ń
                    case 0x83: output += 'N'; break;  // Ń
                    case 0xB3: output += 'o'; break;  // ó
                    case 0x93: output += 'O'; break;  // Ó
                    case 0x9B: output += 's'; break;  // ś
                    case 0x9A: output += 'S'; break;  // Ś
                    case 0xBA: output += 'z'; break;  // ź
                    case 0xB9: output += 'Z'; break;  // Ź
                    case 0xBC: output += 'z'; break;  // ż
                    case 0xBB: output += 'Z'; break;  // Ż
                    default: output += '?'; break;
                }
            }
        }
        else if (c < 128) {
            // Zwykły znak ASCII
            output += (char)c;
        }
        // Inne znaki UTF-8 pomijamy
    }
    
    return output;
}

// ===========================================================================
//                              SETUP
// ===========================================================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n========================================");
    Serial.println("   Zegar i Stacja Pogodowa v1.2");
    Serial.println("   ESP32 + ILI9341 + OpenWeatherMap");
    Serial.println("========================================\n");
    
    // Inicjalizacja pinów
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_BUTTON_1, INPUT);
    pinMode(PIN_BUTTON_2, INPUT);
    pinMode(PIN_BUTTON_3, INPUT_PULLUP);
    pinMode(PIN_LED_STATUS, OUTPUT);
    pinMode(PIN_BACKLIGHT, OUTPUT);
    
    // PWM dla podświetlenia
    #if ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcAttach(PIN_BACKLIGHT, 5000, 8);
    #else
        ledcSetup(0, 5000, 8);
        ledcAttachPin(PIN_BACKLIGHT, 0);
    #endif
    setBacklight(255);
    
    // Inicjalizacja wyświetlacza
    setupDisplay();
    
    // Ekran powitalny
    tft.fillScreen(COLOR_BACKGROUND);
    tft.setTextColor(TFT_CYAN, COLOR_BACKGROUND);
    tft.setTextFont(4);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Stacja Pogodowa", 120, 100);
    tft.setTextColor(TFT_WHITE, COLOR_BACKGROUND);
    tft.setTextFont(2);
    tft.drawString("Laczenie z WiFi...", 120, 140);
    
    // Połączenie WiFi
    setupWiFi();
    
    if (wifiConnected) {
        tft.drawString("Synchronizacja czasu...", 120, 170);
        setupTime();
        
        tft.drawString("Pobieranie pogody...", 120, 200);
        updateWeather();
    }
    
    delay(1500);
    tft.fillScreen(COLOR_BACKGROUND);
    
    // Rysowanie interfejsu
    drawStaticUI();
    drawTime();
    drawDate();
    drawWeather();
    drawStatusBar();
}

// ===========================================================================
//                            GŁÓWNA PĘTLA
// ===========================================================================
void loop() {
    unsigned long currentMillis = millis();
    
    // Aktualizacja wyświetlacza (co sekundę)
    if (currentMillis - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
        lastDisplayUpdate = currentMillis;
        drawTime();
        drawDate();
        drawStatusBar();
    }
    
    // Aktualizacja pogody (co 10 minut)
    if (currentMillis - lastWeatherUpdate >= WEATHER_UPDATE_INTERVAL) {
        lastWeatherUpdate = currentMillis;
        updateWeather();
        drawWeather();
    }
    
    // Synchronizacja NTP (co godzinę)
    if (currentMillis - lastNtpSync >= NTP_SYNC_INTERVAL) {
        lastNtpSync = currentMillis;
        configTime(GMT_OFFSET, DST_OFFSET, NTP_SERVER);
    }
    
    // Sprawdzenie budzika
    if (myAlarm.enabled) {
        checkAlarm();
    }
    
    delay(10);
}

// ===========================================================================
//                       FUNKCJE INICJALIZACYJNE
// ===========================================================================

void setupDisplay() {
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(COLOR_BACKGROUND);
    tft.setTextWrap(false);
    Serial.println("[OK] Wyswietlacz zainicjalizowany");
}

void setupWiFi() {
    Serial.print("[WiFi] Laczenie z: ");
    Serial.println(WIFI_SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
        
        int dotX = 70 + (attempts % 10) * 10;
        tft.fillCircle(dotX, 230, 4, TFT_CYAN);
        if (attempts % 10 == 9) {
            tft.fillRect(70, 226, 100, 10, COLOR_BACKGROUND);
        }
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println("[OK] Polaczono z WiFi!");
        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("[WiFi] Sila sygnalu: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        digitalWrite(PIN_LED_STATUS, HIGH);
    } else {
        wifiConnected = false;
        Serial.println("[BLAD] Nie udalo sie polaczyc z WiFi!");
    }
}

void setupTime() {
    configTime(GMT_OFFSET, DST_OFFSET, NTP_SERVER);
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10000)) {
        timeSync = true;
        Serial.println("[OK] Czas zsynchronizowany");
        Serial.printf("[Czas] %02d:%02d:%02d %02d.%02d.%04d\n",
            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
            timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
    } else {
        timeSync = false;
        Serial.println("[BLAD] Nie udalo sie zsynchronizowac czasu!");
    }
}

// ===========================================================================
//                      STAŁE ELEMENTY INTERFEJSU
// ===========================================================================

void drawStaticUI() {
    tft.drawFastHLine(0, 58, 240, COLOR_SEPARATOR);
    tft.drawFastHLine(0, 88, 240, COLOR_SEPARATOR);
    tft.drawFastHLine(0, 285, 240, COLOR_SEPARATOR);
    
    tft.setTextColor(COLOR_CITY, COLOR_BACKGROUND);
    tft.setTextDatum(TC_DATUM);
    tft.setTextFont(2);
    tft.drawString("POGODA: " + String(OWM_CITY), 120, 93);
}

// ===========================================================================
//                           RYSOWANIE CZASU
// ===========================================================================

void drawTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return;
    }
    
    char timeStr[10];
    sprintf(timeStr, "%02d:%02d:%02d", 
            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    
    if (String(timeStr) != lastTimeStr) {
        lastTimeStr = String(timeStr);
        
        tft.fillRect(0, 2, 240, 54, COLOR_BACKGROUND);
        
        tft.setTextColor(COLOR_TIME, COLOR_BACKGROUND);
        tft.setTextDatum(MC_DATUM);
        tft.setFreeFont(&FreeSansBold24pt7b);
        
        char timeHM[6];
        sprintf(timeHM, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        tft.drawString(timeHM, 105, 32);
        
        tft.setFreeFont(NULL);
        tft.setTextFont(4);
        tft.setTextDatum(ML_DATUM);
        char secStr[4];
        sprintf(secStr, ":%02d", timeinfo.tm_sec);
        tft.drawString(secStr, 195, 32);
    }
}

// ===========================================================================
//                           RYSOWANIE DATY
// ===========================================================================

void drawDate() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return;
    }
    
    const char* daysShort[] = {"Nd", "Pn", "Wt", "Sr", "Cz", "Pt", "So"};
    const char* months[] = {"STY", "LUT", "MAR", "KWI", "MAJ", "CZE", 
                            "LIP", "SIE", "WRZ", "PAZ", "LIS", "GRU"};
    
    char dateStr[20];
    sprintf(dateStr, "%s  %d %s %d",
            daysShort[timeinfo.tm_wday],
            timeinfo.tm_mday,
            months[timeinfo.tm_mon],
            timeinfo.tm_year + 1900);
    
    if (String(dateStr) != lastDateStr) {
        lastDateStr = String(dateStr);
        
        tft.fillRect(0, 60, 240, 27, COLOR_BACKGROUND);
        tft.setTextColor(COLOR_DATE, COLOR_BACKGROUND);
        tft.setTextDatum(MC_DATUM);
        tft.setTextFont(4);
        tft.drawString(dateStr, 120, 73);
    }
}

// ===========================================================================
//                          RYSOWANIE POGODY
// ===========================================================================

void drawWeather() {
    if (!weather.isValid) {
        tft.fillRect(0, 108, 240, 175, COLOR_BACKGROUND);
        tft.setTextColor(TFT_RED, COLOR_BACKGROUND);
        tft.setTextDatum(MC_DATUM);
        tft.setTextFont(4);
        tft.drawString("Brak danych", 120, 180);
        return;
    }
    
    tft.fillRect(0, 108, 240, 175, COLOR_BACKGROUND);
    
    // ============= IKONA POGODY =============
    drawWeatherIcon(5, 115, weather.icon);
    
    // ============= TEMPERATURA =============
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(COLOR_TEMP, COLOR_BACKGROUND);
    tft.setFreeFont(&FreeSansBold24pt7b);
    
    char tempStr[10];
    sprintf(tempStr, "%.0f", weather.temperature);
    tft.drawString(tempStr, 95, 118);
    
    tft.setFreeFont(NULL);
    tft.setTextFont(2);
    int tempWidth = tft.textWidth(tempStr);
    tft.drawString("o", 95 + tempWidth + 45, 118);
    
    tft.setFreeFont(&FreeSansBold18pt7b);
    tft.drawString("C", 95 + tempWidth + 55, 118);
    tft.setFreeFont(NULL);
    
    // Temperatura odczuwalna
    tft.setTextFont(2);
    tft.setTextColor(TFT_LIGHTGREY, COLOR_BACKGROUND);
    char feelsStr[20];
    sprintf(feelsStr, "Odczuw: %.0f C", weather.feelsLike);
    tft.drawString(feelsStr, 95, 165);
    
    // Min / Max
    tft.setTextColor(TFT_DARKGREY, COLOR_BACKGROUND);
    char minMaxStr[25];
    sprintf(minMaxStr, "Min:%.0f  Max:%.0f", weather.tempMin, weather.tempMax);
    tft.drawString(minMaxStr, 95, 182);
    
    // ============= OPIS POGODY (bez polskich znaków) =============
    tft.setTextColor(TFT_WHITE, COLOR_BACKGROUND);
    tft.setTextDatum(TC_DATUM);
    
    // Usuń polskie znaki z opisu
    String desc = removePolishChars(weather.description);
    
    // Pierwsza litera wielka
    if (desc.length() > 0) {
        desc[0] = toupper(desc[0]);
    }
    
    // Sprawdź szerokość
    tft.setTextFont(4);
    int descWidth = tft.textWidth(desc);
    
    if (descWidth > 230) {
        // Za długi - mniejsza czcionka lub 2 linie
        tft.setTextFont(2);
        int spaceIdx = desc.indexOf(' ', desc.length() / 2 - 3);
        if (spaceIdx > 0 && spaceIdx < (int)desc.length() - 2) {
            String line1 = desc.substring(0, spaceIdx);
            String line2 = desc.substring(spaceIdx + 1);
            tft.drawString(line1, 120, 205);
            tft.drawString(line2, 120, 222);
        } else {
            tft.drawString(desc, 120, 212);
        }
    } else {
        tft.drawString(desc, 120, 208);
    }
    
    // ============= DODATKOWE INFO =============
    tft.setTextFont(2);
    tft.setTextDatum(TL_DATUM);
    
    int y1 = 242;
    int y2 = 262;
    
    // Wiersz 1
    tft.setTextColor(COLOR_HUMIDITY, COLOR_BACKGROUND);
    char humStr[20];
    sprintf(humStr, "Wilg: %d%%", weather.humidity);
    tft.drawString(humStr, 10, y1);
    
    tft.setTextColor(COLOR_PRESSURE, COLOR_BACKGROUND);
    char presStr[20];
    sprintf(presStr, "Cisn: %d hPa", weather.pressure);
    tft.drawString(presStr, 125, y1);
    
    // Wiersz 2
    tft.setTextColor(COLOR_WIND, COLOR_BACKGROUND);
    char windStr[25];
    sprintf(windStr, "Wiatr: %.1f m/s %s", weather.windSpeed, getWindDirection(weather.windDeg).c_str());
    tft.drawString(windStr, 10, y2);
    
    tft.setTextColor(TFT_LIGHTGREY, COLOR_BACKGROUND);
    char cloudStr[15];
    sprintf(cloudStr, "Chmury:%d%%", weather.clouds);
    tft.drawString(cloudStr, 165, y2);
}

// ===========================================================================
//                        IKONY POGODOWE
// ===========================================================================

void drawWeatherIcon(int x, int y, String iconCode) {
    const int iconSize = 80;
    
    tft.fillRect(x, y, iconSize, iconSize, COLOR_BACKGROUND);
    
    int cx = x + 40;
    int cy = y + 40;
    
    if (iconCode.startsWith("01")) {
        // SLONCE / KSIEZYC
        if (iconCode.endsWith("d")) {
            tft.fillCircle(cx, cy, 25, TFT_YELLOW);
            for (int i = 0; i < 8; i++) {
                float angle = i * PI / 4;
                int x1 = cx + cos(angle) * 30;
                int y1 = cy + sin(angle) * 30;
                int x2 = cx + cos(angle) * 38;
                int y2 = cy + sin(angle) * 38;
                tft.drawLine(x1, y1, x2, y2, TFT_YELLOW);
                tft.drawLine(x1 + 1, y1, x2 + 1, y2, TFT_YELLOW);
                tft.drawLine(x1, y1 + 1, x2, y2 + 1, TFT_YELLOW);
            }
        } else {
            tft.fillCircle(cx, cy, 25, TFT_LIGHTGREY);
            tft.fillCircle(cx + 12, cy - 6, 20, COLOR_BACKGROUND);
        }
    }
    else if (iconCode.startsWith("02") || iconCode.startsWith("03")) {
        // CZESCIOWE ZACHMURZENIE
        tft.fillCircle(cx + 18, cy - 15, 16, TFT_YELLOW);
        for (int i = 0; i < 5; i++) {
            float angle = -PI/2 + (i - 2) * PI / 6;
            int x1 = cx + 18 + cos(angle) * 20;
            int y1 = cy - 15 + sin(angle) * 20;
            int x2 = cx + 18 + cos(angle) * 26;
            int y2 = cy - 15 + sin(angle) * 26;
            tft.drawLine(x1, y1, x2, y2, TFT_YELLOW);
        }
        tft.fillCircle(cx - 18, cy + 10, 16, TFT_WHITE);
        tft.fillCircle(cx, cy + 2, 20, TFT_WHITE);
        tft.fillCircle(cx + 20, cy + 10, 16, TFT_WHITE);
        tft.fillRect(cx - 18, cy + 10, 40, 20, TFT_WHITE);
    }
    else if (iconCode.startsWith("04")) {
        // CHMURY
        tft.fillCircle(cx - 20, cy + 5, 18, TFT_DARKGREY);
        tft.fillCircle(cx, cy - 8, 22, TFT_LIGHTGREY);
        tft.fillCircle(cx + 20, cy + 5, 18, TFT_DARKGREY);
        tft.fillRect(cx - 20, cy + 5, 44, 22, TFT_LIGHTGREY);
    }
    else if (iconCode.startsWith("09") || iconCode.startsWith("10")) {
        // DESZCZ
        tft.fillCircle(cx - 18, cy - 12, 16, TFT_DARKGREY);
        tft.fillCircle(cx + 2, cy - 20, 18, TFT_LIGHTGREY);
        tft.fillCircle(cx + 20, cy - 12, 16, TFT_DARKGREY);
        tft.fillRect(cx - 18, cy - 12, 40, 16, TFT_LIGHTGREY);
        
        for (int i = 0; i < 5; i++) {
            int dx = cx - 24 + i * 12;
            int dy = cy + 8 + (i % 2) * 10;
            tft.fillCircle(dx, dy + 10, 3, TFT_BLUE);
            tft.fillTriangle(dx - 3, dy + 10, dx + 3, dy + 10, dx, dy, TFT_BLUE);
        }
    }
    else if (iconCode.startsWith("11")) {
        // BURZA
        tft.fillCircle(cx - 18, cy - 15, 16, 0x4208);
        tft.fillCircle(cx, cy - 22, 18, 0x6B4D);
        tft.fillCircle(cx + 18, cy - 15, 16, 0x4208);
        tft.fillRect(cx - 18, cy - 15, 38, 14, 0x6B4D);
        
        tft.fillTriangle(cx + 5, cy - 2, cx - 8, cy + 18, cx + 2, cy + 14, TFT_YELLOW);
        tft.fillTriangle(cx - 4, cy + 10, cx + 10, cy + 10, cx - 6, cy + 35, TFT_YELLOW);
        
        tft.fillCircle(cx - 22, cy + 20, 2, TFT_BLUE);
        tft.fillCircle(cx + 22, cy + 25, 2, TFT_BLUE);
    }
    else if (iconCode.startsWith("13")) {
        // SNIEG
        tft.fillCircle(cx - 18, cy - 15, 16, TFT_LIGHTGREY);
        tft.fillCircle(cx, cy - 22, 18, TFT_WHITE);
        tft.fillCircle(cx + 18, cy - 15, 16, TFT_LIGHTGREY);
        tft.fillRect(cx - 18, cy - 15, 38, 14, TFT_WHITE);
        
        int snowY = cy + 8;
        for (int i = 0; i < 3; i++) {
            int sx = cx - 25 + i * 25;
            int sy = snowY + (i % 2) * 15;
            tft.drawLine(sx - 6, sy, sx + 6, sy, TFT_WHITE);
            tft.drawLine(sx, sy - 6, sx, sy + 6, TFT_WHITE);
            tft.drawLine(sx - 4, sy - 4, sx + 4, sy + 4, TFT_WHITE);
            tft.drawLine(sx + 4, sy - 4, sx - 4, sy + 4, TFT_WHITE);
        }
    }
    else if (iconCode.startsWith("50")) {
        // MGLA
        for (int i = 0; i < 5; i++) {
            int yy = y + 8 + i * 14;
            int width = 70 - abs(i - 2) * 10;
            int xOffset = (80 - width) / 2;
            tft.fillRoundRect(x + xOffset, yy, width, 6, 3, TFT_LIGHTGREY);
        }
    }
    else {
        // NIEZNANA IKONA
        tft.setTextColor(TFT_WHITE, COLOR_BACKGROUND);
        tft.setTextDatum(MC_DATUM);
        tft.setTextFont(4);
        tft.drawString("?", cx, cy - 5);
        tft.setTextFont(2);
        tft.drawString(iconCode, cx, cy + 20);
    }
}

// ===========================================================================
//                          PASEK STATUSU
// ===========================================================================

void drawStatusBar() {
    int y = 290;
    
    tft.fillRect(0, y, 240, 30, COLOR_BACKGROUND);
    
    tft.setTextDatum(TL_DATUM);
    tft.setTextFont(2);
    
    if (WiFi.status() == WL_CONNECTED) {
        int rssi = WiFi.RSSI();
        uint16_t wifiColor;
        String wifiStr;
        
        if (rssi > -50) {
            wifiColor = TFT_GREEN;
            wifiStr = "WiFi OK";
        } else if (rssi > -70) {
            wifiColor = TFT_YELLOW;
            wifiStr = "WiFi ~";
        } else {
            wifiColor = TFT_RED;
            wifiStr = "WiFi !";
        }
        
        tft.setTextColor(wifiColor, COLOR_BACKGROUND);
        tft.drawString(wifiStr, 5, y + 5);
    } else {
        tft.setTextColor(TFT_RED, COLOR_BACKGROUND);
        tft.drawString("OFFLINE", 5, y + 5);
    }
    
    if (myAlarm.enabled) {
        tft.setTextColor(TFT_ORANGE, COLOR_BACKGROUND);
        tft.setTextDatum(TC_DATUM);
        char alarmStr[10];
        sprintf(alarmStr, "%02d:%02d", myAlarm.hour, myAlarm.minute);
        tft.drawString(alarmStr, 120, y + 5);
    }
    
    tft.setTextColor(TFT_DARKGREY, COLOR_BACKGROUND);
    tft.setTextDatum(TR_DATUM);
    unsigned long sinceUpdate = (millis() - lastWeatherUpdate) / 60000;
    char updateStr[15];
    sprintf(updateStr, "%lum", sinceUpdate);
    tft.drawString(updateStr, 235, y + 5);
}

// ===========================================================================
//                       POBIERANIE POGODY
// ===========================================================================

void updateWeather() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[Pogoda] Brak polaczenia WiFi!");
        return;
    }
    
    Serial.println("[Pogoda] Pobieranie danych...");
    
    HTTPClient http;
    http.begin(weatherURL);
    http.setTimeout(10000);
    
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("[Pogoda] Dane otrzymane!");
        parseWeatherData(payload);
    } else {
        Serial.printf("[Pogoda] Blad HTTP: %d\n", httpCode);
        weather.isValid = false;
    }
    
    http.end();
}

// ===========================================================================
//                      PARSOWANIE JSON POGODY
// ===========================================================================

void parseWeatherData(String jsonData) {
    DynamicJsonDocument doc(2048);
    
    DeserializationError error = deserializeJson(doc, jsonData);
    
    if (error) {
        Serial.print("[JSON] Blad parsowania: ");
        Serial.println(error.c_str());
        weather.isValid = false;
        return;
    }
    
    weather.temperature = doc["main"]["temp"];
    weather.tempMin = doc["main"]["temp_min"];
    weather.tempMax = doc["main"]["temp_max"];
    weather.feelsLike = doc["main"]["feels_like"];
    weather.humidity = doc["main"]["humidity"];
    weather.pressure = doc["main"]["pressure"];
    
    weather.windSpeed = doc["wind"]["speed"];
    weather.windDeg = doc["wind"]["deg"] | 0;
    
    weather.clouds = doc["clouds"]["all"];
    weather.visibility = doc["visibility"] | 10000;
    
    weather.description = doc["weather"][0]["description"].as<String>();
    weather.icon = doc["weather"][0]["icon"].as<String>();
    weather.mainCondition = doc["weather"][0]["main"].as<String>();
    
    weather.sunrise = doc["sys"]["sunrise"];
    weather.sunset = doc["sys"]["sunset"];
    
    weather.isValid = true;
    
    // Debug - opis z usuniętymi polskimi znakami
    String cleanDesc = removePolishChars(weather.description);
    Serial.println("[Pogoda] Dane sparsowane:");
    Serial.printf("  Temp: %.1f C (odczuw: %.1f C)\n", weather.temperature, weather.feelsLike);
    Serial.printf("  Wilgotnosc: %d%%, Cisnienie: %d hPa\n", weather.humidity, weather.pressure);
    Serial.printf("  Wiatr: %.1f m/s, kierunek: %d\n", weather.windSpeed, weather.windDeg);
    Serial.printf("  Opis: %s (ikona: %s)\n", cleanDesc.c_str(), weather.icon.c_str());
}

// ===========================================================================
//                        FUNKCJE POMOCNICZE
// ===========================================================================

String getWindDirection(int degrees) {
    const char* directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    int index = ((degrees + 22) % 360) / 45;
    return String(directions[index]);
}

void setBacklight(int level) {
    backlightLevel = constrain(level, 0, 255);
    
    #if ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcWrite(PIN_BACKLIGHT, backlightLevel);
    #else
        ledcWrite(0, backlightLevel);
    #endif
}

// ===========================================================================
//                          BUDZIK
// ===========================================================================

void checkAlarm() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return;
    
    if (timeinfo.tm_hour == myAlarm.hour && 
        timeinfo.tm_min == myAlarm.minute && 
        !myAlarm.triggered) {
        
        int dayBit = (timeinfo.tm_wday == 0) ? 64 : (1 << (timeinfo.tm_wday - 1));
        
        if (myAlarm.daysOfWeek & dayBit) {
            myAlarm.triggered = true;
            triggerAlarm();
        }
    }
    
    if (timeinfo.tm_min != myAlarm.minute) {
        myAlarm.triggered = false;
    }
}

void triggerAlarm() {
    Serial.println("[ALARM] Budzik!");
    
    for (int i = 0; i < 10; i++) {
        digitalWrite(PIN_BUZZER, HIGH);
        tft.fillScreen(TFT_RED);
        delay(200);
        digitalWrite(PIN_BUZZER, LOW);
        tft.fillScreen(COLOR_BACKGROUND);
        delay(200);
    }
    
    tft.fillScreen(COLOR_BACKGROUND);
    drawStaticUI();
    drawTime();
    drawDate();
    drawWeather();
    drawStatusBar();
}

// ===========================================================================
//                         KONIEC PROGRAMU
// ===========================================================================