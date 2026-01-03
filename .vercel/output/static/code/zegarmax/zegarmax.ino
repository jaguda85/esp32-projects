#include <WiFi.h>
#include <WebServer.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <time.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Konfiguracja MAX7219
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   18
#define DATA_PIN  23
#define CS_PIN    5

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
Preferences preferences;

// Konfiguracja WiFi - Access Point
const char* ap_ssid = "";
const char* ap_password = "";

// Konfiguracja WiFi - połączenie z internetem
const char* sta_ssid = "";
const char* sta_password = "";

// Konfiguracja OpenWeatherMap
const char* weatherApiKey = "";
const char* weatherCity = "";
const char* weatherCountry = "PL";

// Serwer NTP i strefa czasowa dla Polski
const char* ntpServer = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const char* timeZone = "CET-1CEST,M3.5.0,M10.5.0/3";

WebServer server(80);

// Polskie nazwy miesięcy (skróty)
const char* monthNamesShort[] = {
  "STY", "LUT", "MAR", "KWI", "MAJ", "CZE",
  "LIP", "SIE", "WRZ", "PAZ", "LIS", "GRU"
};

// Polskie nazwy miesięcy (pełne)
const char* monthNamesFull[] = {
  "Stycznia", "Lutego", "Marca", "Kwietnia", "Maja", "Czerwca",
  "Lipca", "Sierpnia", "Wrzesnia", "Pazdziernika", "Listopada", "Grudnia"
};

// Polskie nazwy dni tygodnia (skróty)
const char* dayNamesShort[] = {
  "NIE", "PON", "WTO", "SRO", "CZW", "PIA", "SOB"
};

// Polskie nazwy dni tygodnia (pełne)
const char* dayNamesFull[] = {
  "Niedziela", "Poniedzialek", "Wtorek", "Sroda", 
  "Czwartek", "Piatek", "Sobota"
};

// Zmienne globalne
String displayText = "";
bool displayingUserText = false;
bool scrolling = false;
uint8_t scrollSpeed = 50;
uint8_t displayIntensity = 5;
unsigned long stopTime = 0;
bool waitingAfterStop = false;

// Tryb wyświetlania daty: 0 = NIERUCHOMA, 1 = SCROLLOWANA
uint8_t dateDisplayMode = 0;

// Stany wyświetlacza
enum DisplayState {
  STATE_TIME,      // Wyświetla czas
  STATE_DAYNAME,   // Wyświetla nazwę dnia
  STATE_DATE,      // Wyświetla datę
  STATE_WEATHER    // Wyświetla pogodę
};

DisplayState currentState = STATE_TIME;
unsigned long lastSwitch = 0;
const unsigned long TIME_DISPLAY_DURATION = 30000;  // 30 sekund
const unsigned long DAY_DISPLAY_DURATION = 5000;    // 5 sekund
const unsigned long DATE_DISPLAY_DURATION = 5000;   // 5 sekund

// Zmienna dla mrugającego dwukropka
bool colonVisible = true;
unsigned long lastColonBlink = 0;
const unsigned long COLON_BLINK_INTERVAL = 500;

// Zmienne dla scrollowania
bool isScrollingDate = false;
bool isScrollingWeather = false;
char scrollBuffer[300];

// Zmienne pogodowe
unsigned long lastWeatherUpdate = 0;
const unsigned long WEATHER_UPDATE_INTERVAL = 180000; // 3 minuty
float currentTemp = 0;
float feelsLike = 0;
int humidity = 0;
int pressure = 0;
int clouds = 0;
float windSpeed = 0;
String weatherDesc = "";
String weatherMain = "";
float rain1h = 0;
float snow1h = 0;
bool weatherDataValid = false;
String weatherAlert = "";

// Ustawienia wyświetlania pogody (bitmask)
struct WeatherSettings {
  bool showTemp = true;
  bool showFeelsLike = false;
  bool showClouds = true;
  bool showRain = true;
  bool showHumidity = false;
  bool showPressure = false;
  bool showWind = false;
  bool showDescription = true;
};

WeatherSettings weatherSettings;

// Funkcje zapisu/odczytu ustawień
void loadSettings() {
  preferences.begin("display", false);
  
  scrollSpeed = preferences.getUChar("speed", 50);
  displayIntensity = preferences.getUChar("brightness", 5);
  dateDisplayMode = preferences.getUChar("dateMode", 0);
  
  // Wczytaj ustawienia pogody
  weatherSettings.showTemp = preferences.getBool("w_temp", true);
  weatherSettings.showFeelsLike = preferences.getBool("w_feels", false);
  weatherSettings.showClouds = preferences.getBool("w_clouds", true);
  weatherSettings.showRain = preferences.getBool("w_rain", true);
  weatherSettings.showHumidity = preferences.getBool("w_humid", false);
  weatherSettings.showPressure = preferences.getBool("w_press", false);
  weatherSettings.showWind = preferences.getBool("w_wind", false);
  weatherSettings.showDescription = preferences.getBool("w_desc", true);
  
  preferences.end();
  
  Serial.println("=== Wczytane ustawienia ===");
  Serial.printf("Predkosc: %d\n", scrollSpeed);
  Serial.printf("Jasnosc: %d\n", displayIntensity);
  Serial.printf("Tryb daty: %s\n", dateDisplayMode == 0 ? "NIERUCHOMA" : "SCROLLOWANA");
  Serial.println("Pogoda: temp=" + String(weatherSettings.showTemp) + 
                 ", clouds=" + String(weatherSettings.showClouds) + 
                 ", rain=" + String(weatherSettings.showRain));
}

void saveSettings() {
  preferences.begin("display", false);
  
  preferences.putUChar("speed", scrollSpeed);
  preferences.putUChar("brightness", displayIntensity);
  preferences.putUChar("dateMode", dateDisplayMode);
  
  // Zapisz ustawienia pogody
  preferences.putBool("w_temp", weatherSettings.showTemp);
  preferences.putBool("w_feels", weatherSettings.showFeelsLike);
  preferences.putBool("w_clouds", weatherSettings.showClouds);
  preferences.putBool("w_rain", weatherSettings.showRain);
  preferences.putBool("w_humid", weatherSettings.showHumidity);
  preferences.putBool("w_press", weatherSettings.showPressure);
  preferences.putBool("w_wind", weatherSettings.showWind);
  preferences.putBool("w_desc", weatherSettings.showDescription);
  
  preferences.end();
  
  Serial.println("=== Zapisano ustawienia ===");
}

// Funkcja pobierania aktualnej pogody
void fetchWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Brak WiFi - nie moge pobrac pogody");
    return;
  }
  
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + 
               String(weatherCity) + "," + String(weatherCountry) + 
               "&appid=" + String(weatherApiKey) + 
               "&units=metric&lang=pl";
  
  Serial.println("Pobieram pogode: " + url);
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      currentTemp = doc["main"]["temp"];
      feelsLike = doc["main"]["feels_like"];
      humidity = doc["main"]["humidity"];
      pressure = doc["main"]["pressure"];
      clouds = doc["clouds"]["all"];
      windSpeed = doc["wind"]["speed"];
      weatherDesc = doc["weather"][0]["description"].as<String>();
      weatherMain = doc["weather"][0]["main"].as<String>();
      
      // Opady
      rain1h = doc["rain"]["1h"] | 0.0;
      snow1h = doc["snow"]["1h"] | 0.0;
      
      weatherDataValid = true;
      
      Serial.println("=== Pobrano pogode ===");
      Serial.printf("Temp: %.1f°C (odczuwalna: %.1f°C)\n", currentTemp, feelsLike);
      Serial.printf("Zachmurzenie: %d%%\n", clouds);
      Serial.printf("Opis: %s\n", weatherDesc.c_str());
      if (rain1h > 0) Serial.printf("Deszcz: %.1fmm\n", rain1h);
      if (snow1h > 0) Serial.printf("Snieg: %.1fmm\n", snow1h);
      
      // Pobierz prognozę
      fetchForecast();
    } else {
      Serial.println("Blad parsowania JSON pogody");
    }
  } else {
    Serial.printf("Blad HTTP: %d\n", httpCode);
  }
  
  http.end();
}

// Funkcja pobierania prognozy (3h)
void fetchForecast() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/forecast?q=" + 
               String(weatherCity) + "," + String(weatherCountry) + 
               "&appid=" + String(weatherApiKey) + 
               "&units=metric&lang=pl&cnt=1";
  
  http.begin(url);
  int httpCode = http.GET();
  
  weatherAlert = "";  // Reset alertu
  
  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      float forecastRain = doc["list"][0]["rain"]["3h"] | 0.0;
      float forecastSnow = doc["list"][0]["snow"]["3h"] | 0.0;
      String forecastMain = doc["list"][0]["weather"][0]["main"].as<String>();
      
      // Sprawdź zmiany pogody w ciągu 3h
      if (forecastRain > 0 && rain1h == 0) {
        weatherAlert = "Za 3h przewidywane opady deszczu";
        Serial.println("ALERT: " + weatherAlert);
      }
      else if (forecastSnow > 0 && snow1h == 0) {
        weatherAlert = "Za 3h przewidywany snieg";
        Serial.println("ALERT: " + weatherAlert);
      }
      else if (forecastMain != weatherMain) {
        weatherAlert = "Za 3h zmiana pogody";
        Serial.println("ALERT: " + weatherAlert);
      }
    }
  }
  
  http.end();
}

// Funkcja generująca tekst pogody
String getWeatherText() {
  if (!weatherDataValid) {
    return "Brak danych pogodowych";
  }
  
  String weatherText = "";
  
  // Temperatura
  if (weatherSettings.showTemp) {
    weatherText += String((int)round(currentTemp)) + "st.C";
  }
  
  // Temperatura odczuwalna
  if (weatherSettings.showFeelsLike) {
    if (weatherText.length() > 0) weatherText += " --> ";
    weatherText += "Odczuw. " + String((int)round(feelsLike)) + "st.C";
  }
  
  // Zachmurzenie
  if (weatherSettings.showClouds) {
    if (weatherText.length() > 0) weatherText += "  -->  ";
    if (clouds < 20) weatherText += "Bezchmurnie";
    else if (clouds < 50) weatherText += "Malo chmur " + String(clouds) + "%";
    else if (clouds < 80) weatherText += "Zachmurzenie " + String(clouds) + "%";
    else weatherText += "Bardzo pochmurno " + String(clouds) + "%";
  }
  
  // Opady
  if (weatherSettings.showRain) {
    if (rain1h > 0) {
      if (weatherText.length() > 0) weatherText += "   -->  ";
      weatherText += "Deszcz " + String(rain1h, 1) + "mm";
    }
    if (snow1h > 0) {
      if (weatherText.length() > 0) weatherText += "  -->  ";
      weatherText += "Snieg " + String(snow1h, 1) + "mm";
    }
    if (rain1h == 0 && snow1h == 0) {
      if (weatherText.length() > 0) weatherText += "  -->  ";
      weatherText += "Brak opadow";
    }
  }
  
  // Wilgotność
  if (weatherSettings.showHumidity) {
    if (weatherText.length() > 0) weatherText += "  -->  ";
    weatherText += "Wilgotnosc " + String(humidity) + "%";
  }
  
  // Ciśnienie
  if (weatherSettings.showPressure) {
    if (weatherText.length() > 0) weatherText += "  -->  ";
    weatherText += "Cisnienie " + String(pressure) + "hPa";
  }
  
  // Wiatr
  if (weatherSettings.showWind) {
    if (weatherText.length() > 0) weatherText += "  -->  ";
    weatherText += "Wiatr " + String(windSpeed, 1) + "m/s";
  }
  
  // Opis
  if (weatherSettings.showDescription) {
    if (weatherText.length() > 0) weatherText += "  -->  ";
    weatherText += weatherDesc;
  }
  
  // Alert o zmianie pogody
  if (weatherAlert.length() > 0) {
    weatherText += "   --->>  !!! " + weatherAlert + " !!!";
  }
  
  return weatherText;
}

// Strona HTML
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Wyświetlacz LED</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 15px;
        }
        
        .container {
            max-width: 600px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 20px;
            text-align: center;
        }
        
        .header h1 {
            font-size: 24px;
            font-weight: 600;
        }
        
        .content {
            padding: 20px;
        }
        
        .section {
            margin-bottom: 20px;
        }
        
        .section-title {
            font-size: 14px;
            font-weight: 600;
            color: #666;
            margin-bottom: 10px;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        
        textarea {
            width: 100%;
            min-height: 100px;
            padding: 12px;
            border: 2px solid #e0e0e0;
            border-radius: 10px;
            font-size: 16px;
            resize: vertical;
            font-family: inherit;
            transition: border-color 0.3s;
        }
        
        textarea:focus {
            outline: none;
            border-color: #667eea;
        }
        
        .char-count {
            text-align: right;
            color: #999;
            font-size: 13px;
            margin-top: 5px;
        }
        
        .btn-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            margin-top: 15px;
        }
        
        .btn-grid.three-col {
            grid-template-columns: repeat(3, 1fr);
        }
        
        .btn-grid.full {
            grid-template-columns: 1fr;
        }
        
        button {
            padding: 15px;
            font-size: 15px;
            font-weight: 600;
            border: none;
            border-radius: 10px;
            cursor: pointer;
            transition: all 0.3s;
            text-transform: uppercase;
            letter-spacing: 0.5px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        
        button:active {
            transform: translateY(2px);
            box-shadow: 0 2px 3px rgba(0,0,0,0.1);
        }
        
        .btn-send { background: #4CAF50; color: white; }
        .btn-stop { background: #ff9800; color: white; }
        .btn-clear { background: #f44336; color: white; }
        .btn-date { background: #2196F3; color: white; }
        .btn-weather { background: #00BCD4; color: white; }
        .btn-save { background: #9C27B0; color: white; }
        
        .date-mode, .weather-box {
            background: #f8f9fa;
            padding: 15px;
            border-radius: 10px;
            border: 2px solid #e0e0e0;
        }
        
        .weather-box {
            border-color: #00BCD4;
            background: #e0f7fa;
        }
        
        .radio-group {
            display: flex;
            gap: 10px;
            margin-top: 10px;
        }
        
        .radio-option {
            flex: 1;
            position: relative;
        }
        
        .radio-option input[type="radio"] {
            position: absolute;
            opacity: 0;
        }
        
        .radio-option label {
            display: block;
            padding: 12px;
            background: white;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            text-align: center;
            cursor: pointer;
            transition: all 0.3s;
            font-weight: 600;
            font-size: 14px;
        }
        
        .radio-option input[type="radio"]:checked + label {
            background: #667eea;
            color: white;
            border-color: #667eea;
        }
        
        .mode-desc {
            margin-top: 10px;
            font-size: 12px;
            color: #666;
            text-align: center;
            line-height: 1.4;
        }
        
        .checkbox-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            margin-top: 10px;
        }
        
        .checkbox-option {
            display: flex;
            align-items: center;
            padding: 10px;
            background: white;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.3s;
        }
        
        .checkbox-option:hover {
            border-color: #00BCD4;
        }
        
        .checkbox-option input[type="checkbox"] {
            width: 20px;
            height: 20px;
            margin-right: 8px;
            cursor: pointer;
        }
        
        .checkbox-option input[type="checkbox"]:checked + label {
            color: #00BCD4;
            font-weight: 600;
        }
        
        .checkbox-option label {
            cursor: pointer;
            font-size: 14px;
            flex: 1;
        }
        
        .slider-box {
            background: #f8f9fa;
            padding: 15px;
            border-radius: 10px;
            border: 2px solid #e0e0e0;
        }
        
        .slider-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 10px;
        }
        
        .slider-label {
            font-weight: 600;
            font-size: 14px;
            color: #333;
        }
        
        .slider-value {
            background: #667eea;
            color: white;
            padding: 4px 12px;
            border-radius: 20px;
            font-weight: 600;
            font-size: 14px;
        }
        
        input[type="range"] {
            width: 100%;
            height: 6px;
            border-radius: 5px;
            background: #ddd;
            outline: none;
            -webkit-appearance: none;
        }
        
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 24px;
            height: 24px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            box-shadow: 0 2px 5px rgba(0,0,0,0.2);
        }
        
        input[type="range"]::-moz-range-thumb {
            width: 24px;
            height: 24px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            border: none;
            box-shadow: 0 2px 5px rgba(0,0,0,0.2);
        }
        
        .slider-hint {
            font-size: 11px;
            color: #999;
            margin-top: 5px;
            text-align: center;
        }
        
        .status {
            padding: 12px;
            border-radius: 10px;
            text-align: center;
            font-weight: 600;
            display: none;
            margin-top: 15px;
            animation: slideIn 0.3s ease;
        }
        
        @keyframes slideIn {
            from { opacity: 0; transform: translateY(-10px); }
            to { opacity: 1; transform: translateY(0); }
        }
        
        .status.show { display: block; }
        .status.success { background: #d4edda; color: #155724; }
        .status.error { background: #f8d7da; color: #721c24; }
        
        .info-box {
            background: #e3f2fd;
            padding: 12px;
            border-radius: 10px;
            border-left: 4px solid #2196F3;
            font-size: 13px;
            color: #0d47a1;
            line-height: 1.5;
        }
        
        .save-box {
            background: #f3e5f5;
            padding: 15px;
            border-radius: 10px;
            border: 2px solid #9C27B0;
        }
        
        .save-info {
            font-size: 12px;
            color: #6a1b9a;
            margin-bottom: 10px;
            text-align: center;
            line-height: 1.4;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔤 Wyświetlacz LED</h1>
        </div>
        
        <div class="content">
            <!-- Tekst -->
            <div class="section">
                <div class="section-title">Tekst do wyświetlenia</div>
                <textarea id="textInput" maxlength="250" placeholder="Wpisz tekst..."></textarea>
                <div class="char-count"><span id="charCount">0</span> / 250 znaków</div>
            </div>
            
            <!-- Przyciski akcji -->
            <div class="section">
                <div class="btn-grid three-col">
                    <button class="btn-send" onclick="sendText()">Wyślij</button>
                    <button class="btn-stop" onclick="stopScroll()">Stop</button>
                    <button class="btn-clear" onclick="clearText()">Wyczyść</button>
                </div>
                <div class="btn-grid" style="margin-top: 10px;">
                    <button class="btn-date" onclick="showDateNow()">📅 Wyświetl datę</button>
                    <button class="btn-weather" onclick="showWeatherNow()">🌤️ Wyświetl pogodę</button>
                </div>
            </div>
            
            <!-- Tryb daty -->
            <div class="section">
                <div class="section-title">Tryb wyświetlania daty</div>
                <div class="date-mode">
                    <div class="radio-group">
                        <div class="radio-option">
                            <input type="radio" id="modeStatic" name="dateMode" value="0" checked onchange="changeDateMode(0)">
                            <label for="modeStatic">Nieruchoma</label>
                        </div>
                        <div class="radio-option">
                            <input type="radio" id="modeScroll" name="dateMode" value="1" onchange="changeDateMode(1)">
                            <label for="modeScroll">Scrollowana</label>
                        </div>
                    </div>
                    <div class="mode-desc" id="modeDesc">30s zegar → 5s dzień → 5s data</div>
                </div>
            </div>
            
            <!-- Ustawienia pogody -->
            <div class="section">
                <div class="section-title">🌤️ Parametry pogody (Gniezno)</div>
                <div class="weather-box">
                    <div class="checkbox-grid">
                        <div class="checkbox-option">
                            <input type="checkbox" id="w_temp" checked onchange="updateWeatherSettings()">
                            <label for="w_temp">Temperatura</label>
                        </div>
                        <div class="checkbox-option">
                            <input type="checkbox" id="w_feels" onchange="updateWeatherSettings()">
                            <label for="w_feels">Odczuwalna</label>
                        </div>
                        <div class="checkbox-option">
                            <input type="checkbox" id="w_clouds" checked onchange="updateWeatherSettings()">
                            <label for="w_clouds">Zachmurzenie</label>
                        </div>
                        <div class="checkbox-option">
                            <input type="checkbox" id="w_rain" checked onchange="updateWeatherSettings()">
                            <label for="w_rain">Opady</label>
                        </div>
                        <div class="checkbox-option">
                            <input type="checkbox" id="w_humid" onchange="updateWeatherSettings()">
                            <label for="w_humid">Wilgotność</label>
                        </div>
                        <div class="checkbox-option">
                            <input type="checkbox" id="w_press" onchange="updateWeatherSettings()">
                            <label for="w_press">Ciśnienie</label>
                        </div>
                        <div class="checkbox-option">
                            <input type="checkbox" id="w_wind" onchange="updateWeatherSettings()">
                            <label for="w_wind">Wiatr</label>
                        </div>
                        <div class="checkbox-option">
                            <input type="checkbox" id="w_desc" checked onchange="updateWeatherSettings()">
                            <label for="w_desc">Opis</label>
                        </div>
                    </div>
                    <div class="mode-desc" style="margin-top: 10px; color: #006064;">
                        Pogoda wyświetlana automatycznie co 3 min. (priorytet nad datą)
                    </div>
                </div>
            </div>
            
            <!-- Ustawienia -->
            <div class="section">
                <div class="section-title">Ustawienia</div>
                <div class="slider-box" style="margin-bottom: 15px;">
                    <div class="slider-header">
                        <span class="slider-label">⚡ Prędkość</span>
                        <span class="slider-value" id="speedValue">50</span>
                    </div>
                    <input type="range" id="speedSlider" min="10" max="150" value="50" oninput="updateSpeed(this.value)">
                    <div class="slider-hint">← Szybciej | Wolniej →</div>
                </div>
                
                <div class="slider-box">
                    <div class="slider-header">
                        <span class="slider-label">💡 Jasność</span>
                        <span class="slider-value" id="brightnessValue">5</span>
                    </div>
                    <input type="range" id="brightnessSlider" min="0" max="15" value="5" oninput="updateBrightness(this.value)">
                    <div class="slider-hint">0 = min | 15 = max</div>
                </div>
            </div>
            
            <!-- Zapisz -->
            <div class="section">
                <div class="save-box">
                    <div class="save-info">
                        💾 Zapisz wszystkie ustawienia na stałe<br>
                        (prędkość, jasność, tryb daty, parametry pogody)
                    </div>
                    <button class="btn-save" style="width: 100%;" onclick="saveSettings()">
                        💾 Zapisz ustawienia
                    </button>
                </div>
            </div>
            
            <div id="status" class="status"></div>
            
            <div class="section">
                <div class="info-box">
                    ℹ️ Tryb automatyczny: zegar → pogoda (co 3 min) → data
                </div>
            </div>
        </div>
    </div>

    <script>
        const textInput = document.getElementById('textInput');
        const charCount = document.getElementById('charCount');
        const statusDiv = document.getElementById('status');

        textInput.addEventListener('input', function() {
            charCount.textContent = this.value.length;
        });

        function showStatus(message, isSuccess) {
            statusDiv.textContent = message;
            statusDiv.className = 'status show ' + (isSuccess ? 'success' : 'error');
            setTimeout(() => { statusDiv.className = 'status'; }, 3000);
        }

        function changeDateMode(mode) {
            document.getElementById('modeDesc').textContent = mode == 0 ? 
                '30s zegar → 5s dzień → 5s data' : '30s zegar → scrollowanie pełnej daty';
            fetch('/datemode?value=' + mode).then(() => showStatus('✓ Tryb zmieniony', true));
        }

        function sendText() {
            const text = textInput.value;
            if (text.trim() === '') { showStatus('⚠️ Wpisz tekst!', false); return; }
            fetch('/send', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: 'text=' + encodeURIComponent(text)
            }).then(() => showStatus('✓ Tekst wysłany!', true));
        }

        function stopScroll() {
            fetch('/stop').then(() => showStatus('⏸ Zatrzymano', true));
        }

        function clearText() {
            textInput.value = '';
            charCount.textContent = '0';
            showStatus('✓ Pole wyczyszczone', true);
        }

        function showDateNow() {
            fetch('/showdate').then(() => showStatus('📅 Data wyświetlona', true));
        }

        function showWeatherNow() {
            fetch('/showweather').then(() => showStatus('🌤️ Pogoda wyświetlona', true));
        }

        function updateWeatherSettings() {
            const settings = {
                temp: document.getElementById('w_temp').checked,
                feels: document.getElementById('w_feels').checked,
                clouds: document.getElementById('w_clouds').checked,
                rain: document.getElementById('w_rain').checked,
                humid: document.getElementById('w_humid').checked,
                press: document.getElementById('w_press').checked,
                wind: document.getElementById('w_wind').checked,
                desc: document.getElementById('w_desc').checked
            };
            fetch('/weathersettings', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(settings)
            });
        }

        function saveSettings() {
            fetch('/savesettings').then(() => showStatus('💾 Ustawienia zapisane!', true));
        }

        function updateSpeed(value) {
            document.getElementById('speedValue').textContent = value;
            fetch('/speed?value=' + value);
        }

        function updateBrightness(value) {
            document.getElementById('brightnessValue').textContent = value;
            fetch('/brightness?value=' + value);
        }

        window.onload = function() {
            fetch('/settings')
            .then(response => response.json())
            .then(data => {
                document.getElementById('speedSlider').value = data.speed;
                document.getElementById('brightnessSlider').value = data.brightness;
                document.getElementById('speedValue').textContent = data.speed;
                document.getElementById('brightnessValue').textContent = data.brightness;
                
                if (data.dateMode == 0) document.getElementById('modeStatic').checked = true;
                else document.getElementById('modeScroll').checked = true;
                
                document.getElementById('w_temp').checked = data.w_temp;
                document.getElementById('w_feels').checked = data.w_feels;
                document.getElementById('w_clouds').checked = data.w_clouds;
                document.getElementById('w_rain').checked = data.w_rain;
                document.getElementById('w_humid').checked = data.w_humid;
                document.getElementById('w_press').checked = data.w_press;
                document.getElementById('w_wind').checked = data.w_wind;
                document.getElementById('w_desc').checked = data.w_desc;
            });
        }
    </script>
</body>
</html>
)rawliteral";

// Handlery serwera
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSend() {
  if (server.hasArg("text")) {
    displayText = server.arg("text");
    displayingUserText = true;
    scrolling = true;
    waitingAfterStop = false;
    isScrollingDate = false;
    isScrollingWeather = false;
    myDisplay.displayClear();
    myDisplay.setSpeed(scrollSpeed);
    myDisplay.displayText(displayText.c_str(), PA_CENTER, scrollSpeed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Brak tekstu");
  }
}

void handleStop() {
  displayingUserText = false;
  scrolling = false;
  isScrollingDate = false;
  isScrollingWeather = false;
  waitingAfterStop = true;
  stopTime = millis();
  myDisplay.displayClear();
  server.send(200, "text/plain", "OK");
}

void handleClear() {
  server.send(200, "text/plain", "OK");
}

void handleShowDate() {
  displayingUserText = false;
  scrolling = false;
  waitingAfterStop = false;
  isScrollingWeather = false;
  
  if (dateDisplayMode == 0) {
    isScrollingDate = false;
    currentState = STATE_DAYNAME;
    lastSwitch = millis();
    String dayName = getDayNameShort();
    myDisplay.displayClear();
    myDisplay.setTextAlignment(PA_CENTER);
    myDisplay.print(dayName.c_str());
  } else {
    String fullDate = getDateFull();
    fullDate.toCharArray(scrollBuffer, sizeof(scrollBuffer));
    isScrollingDate = true;
    currentState = STATE_TIME;
    lastSwitch = millis();
    myDisplay.displayClear();
    delay(50);
    myDisplay.displayText(scrollBuffer, PA_CENTER, scrollSpeed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }
  server.send(200, "text/plain", "OK");
}

void handleShowWeather() {
  displayingUserText = false;
  scrolling = false;
  waitingAfterStop = false;
  isScrollingDate = false;
  
  // Pobierz aktualną pogodę
  fetchWeather();
  
  String weatherText = getWeatherText();
  weatherText.toCharArray(scrollBuffer, sizeof(scrollBuffer));
  
  isScrollingWeather = true;
  currentState = STATE_TIME;
  lastSwitch = millis();
  lastWeatherUpdate = millis(); // Reset timera
  
  myDisplay.displayClear();
  delay(50);
  myDisplay.displayText(scrollBuffer, PA_CENTER, scrollSpeed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  
  Serial.println("Wyswietlam pogode na zadanie");
  server.send(200, "text/plain", "OK");
}

void handleSpeed() {
  if (server.hasArg("value")) {
    scrollSpeed = server.arg("value").toInt();
    myDisplay.setSpeed(scrollSpeed);
    server.send(200, "text/plain", "OK");
  }
}

void handleBrightness() {
  if (server.hasArg("value")) {
    displayIntensity = server.arg("value").toInt();
    myDisplay.setIntensity(displayIntensity);
    server.send(200, "text/plain", "OK");
  }
}

void handleDateMode() {
  if (server.hasArg("value")) {
    dateDisplayMode = server.arg("value").toInt();
    currentState = STATE_TIME;
    isScrollingDate = false;
    isScrollingWeather = false;
    lastSwitch = millis();
    myDisplay.displayClear();
    server.send(200, "text/plain", "OK");
  }
}

void handleWeatherSettings() {
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(512);
    deserializeJson(doc, server.arg("plain"));
    
    weatherSettings.showTemp = doc["temp"];
    weatherSettings.showFeelsLike = doc["feels"];
    weatherSettings.showClouds = doc["clouds"];
    weatherSettings.showRain = doc["rain"];
    weatherSettings.showHumidity = doc["humid"];
    weatherSettings.showPressure = doc["press"];
    weatherSettings.showWind = doc["wind"];
    weatherSettings.showDescription = doc["desc"];
    
    server.send(200, "text/plain", "OK");
  }
}

void handleSaveSettings() {
  saveSettings();
  server.send(200, "text/plain", "OK");
}

void handleSettings() {
  String json = "{\"speed\":" + String(scrollSpeed) + 
                ",\"brightness\":" + String(displayIntensity) + 
                ",\"dateMode\":" + String(dateDisplayMode) +
                ",\"w_temp\":" + String(weatherSettings.showTemp ? "true" : "false") +
                ",\"w_feels\":" + String(weatherSettings.showFeelsLike ? "true" : "false") +
                ",\"w_clouds\":" + String(weatherSettings.showClouds ? "true" : "false") +
                ",\"w_rain\":" + String(weatherSettings.showRain ? "true" : "false") +
                ",\"w_humid\":" + String(weatherSettings.showHumidity ? "true" : "false") +
                ",\"w_press\":" + String(weatherSettings.showPressure ? "true" : "false") +
                ",\"w_wind\":" + String(weatherSettings.showWind ? "true" : "false") +
                ",\"w_desc\":" + String(weatherSettings.showDescription ? "true" : "false") +
                "}";
  server.send(200, "application/json", json);
}

// Funkcje pomocnicze
String getTimeString(bool showColon) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return "??:??";
  char timeStr[10];
  strftime(timeStr, sizeof(timeStr), showColon ? "%H:%M" : "%H %M", &timeinfo);
  return String(timeStr);
}

String getDayNameShort() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return "???";
  return String(dayNamesShort[timeinfo.tm_wday]);
}

String getDateShort() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return "?? ???";
  char dayStr[3];
  sprintf(dayStr, "%02d", timeinfo.tm_mday);
  return String(dayStr) + " " + String(monthNamesShort[timeinfo.tm_mon]);
}

String getDateFull() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return "Brak czasu";
  char dayStr[3];
  sprintf(dayStr, "%02d", timeinfo.tm_mday);
  int year = timeinfo.tm_year + 1900;
  return String(dayNamesFull[timeinfo.tm_wday]) + " " + String(dayStr) + " " + 
         String(monthNamesFull[timeinfo.tm_mon]) + " " + String(year) + "r";
}

void setup() {
  Serial.begin(115200);
  loadSettings();
  
  myDisplay.begin();
  myDisplay.setIntensity(displayIntensity);
  myDisplay.setSpeed(scrollSpeed);
  myDisplay.displayClear();
  myDisplay.print("Start...");
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress apIP = WiFi.softAPIP();
  
  Serial.println("\n=== AP uruchomiony ===");
  Serial.println("IP: " + apIP.toString());
  
  WiFi.begin(sta_ssid, sta_password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[OK] WiFi polaczone");
    configTzTime(timeZone, ntpServer, ntpServer2);
    
    // Pobierz pierwszą pogodę
    delay(2000);
    fetchWeather();
  }
  
  server.on("/", handleRoot);
  server.on("/send", HTTP_POST, handleSend);
  server.on("/stop", handleStop);
  server.on("/clear", handleClear);
  server.on("/showdate", handleShowDate);
  server.on("/showweather", handleShowWeather);
  server.on("/speed", handleSpeed);
  server.on("/brightness", handleBrightness);
  server.on("/datemode", handleDateMode);
  server.on("/weathersettings", HTTP_POST, handleWeatherSettings);
  server.on("/savesettings", handleSaveSettings);
  server.on("/settings", handleSettings);
  server.begin();
  
  Serial.println("Serwer uruchomiony: http://" + apIP.toString());
  
  myDisplay.displayClear();
  lastSwitch = millis();
  lastColonBlink = millis();
  lastWeatherUpdate = millis();
}

void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();
  
  // Mrugający dwukropek
  if (currentMillis - lastColonBlink >= COLON_BLINK_INTERVAL) {
    lastColonBlink = currentMillis;
    colonVisible = !colonVisible;
  }
  
  // Automatyczne pobieranie pogody co 3 minuty
  if (currentMillis - lastWeatherUpdate >= WEATHER_UPDATE_INTERVAL && !displayingUserText) {
    Serial.println("=== Auto-update pogody ===");
    fetchWeather();
    lastWeatherUpdate = currentMillis;
    
    // Wyświetl pogodę (priorytet!)
    String weatherText = getWeatherText();
    weatherText.toCharArray(scrollBuffer, sizeof(scrollBuffer));
    isScrollingWeather = true;
    isScrollingDate = false;
    currentState = STATE_TIME;
    lastSwitch = currentMillis;
    myDisplay.displayClear();
    delay(50);
    myDisplay.displayText(scrollBuffer, PA_CENTER, scrollSpeed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }
  
  // STOP
  if (waitingAfterStop && (currentMillis - stopTime > 1000)) {
    waitingAfterStop = false;
    lastSwitch = currentMillis;
    currentState = STATE_TIME;
    isScrollingDate = false;
    isScrollingWeather = false;
  }
  
  // Tekst użytkownika
  if (displayingUserText && scrolling) {
    if (myDisplay.displayAnimate()) myDisplay.displayReset();
  }
  // Scrollowanie pogody
  else if (isScrollingWeather) {
    if (myDisplay.displayAnimate()) {
      myDisplay.displayReset();
      isScrollingWeather = false;
      currentState = STATE_TIME;
      lastSwitch = currentMillis;
      myDisplay.displayClear();
    }
  }
  // Scrollowanie daty
  else if (isScrollingDate) {
    if (myDisplay.displayAnimate()) {
      myDisplay.displayReset();
      isScrollingDate = false;
      currentState = STATE_TIME;
      lastSwitch = currentMillis;
      myDisplay.displayClear();
    }
  }
  // Normalny tryb zegara/daty
  else if (!displayingUserText && !waitingAfterStop && !isScrollingDate && !isScrollingWeather) {
    if (dateDisplayMode == 0) {
      // NIERUCHOMA
      if (currentState == STATE_TIME && (currentMillis - lastSwitch >= TIME_DISPLAY_DURATION)) {
        currentState = STATE_DAYNAME;
        lastSwitch = currentMillis;
        String dayName = getDayNameShort();
        myDisplay.displayClear();
        myDisplay.setTextAlignment(PA_CENTER);
        myDisplay.print(dayName.c_str());
      }
      else if (currentState == STATE_DAYNAME && (currentMillis - lastSwitch >= DAY_DISPLAY_DURATION)) {
        currentState = STATE_DATE;
        lastSwitch = currentMillis;
        String dateStr = getDateShort();
        myDisplay.displayClear();
        myDisplay.setTextAlignment(PA_CENTER);
        myDisplay.print(dateStr.c_str());
      }
      else if (currentState == STATE_DATE && (currentMillis - lastSwitch >= DATE_DISPLAY_DURATION)) {
        currentState = STATE_TIME;
        lastSwitch = currentMillis;
      }
      
      if (currentState == STATE_TIME) {
        static bool lastColonState = true;
        if (lastColonState != colonVisible) {
          lastColonState = colonVisible;
          String timeStr = getTimeString(colonVisible);
          myDisplay.displayClear();
          myDisplay.setTextAlignment(PA_CENTER);
          myDisplay.print(timeStr.c_str());
        }
      }
    }
    else {
      // SCROLLOWANA
      if (currentMillis - lastSwitch >= TIME_DISPLAY_DURATION) {
        String fullDate = getDateFull();
        fullDate.toCharArray(scrollBuffer, sizeof(scrollBuffer));
        isScrollingDate = true;
        lastSwitch = currentMillis;
        myDisplay.displayClear();
        delay(50);
        myDisplay.displayText(scrollBuffer, PA_CENTER, scrollSpeed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      }
      else {
        static bool lastColonState = true;
        if (lastColonState != colonVisible) {
          lastColonState = colonVisible;
          String timeStr = getTimeString(colonVisible);
          myDisplay.displayClear();
          myDisplay.setTextAlignment(PA_CENTER);
          myDisplay.print(timeStr.c_str());
        }
      }
    }
  }
}