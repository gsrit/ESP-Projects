#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <TFT_eSPI.h>
#include <time.h>

// --- Configuration ---
const char* ssid = "SSID";
const char* password = "PASS";
const char* url = "https://solar.w5mmw.net/";

#define BUTTON1 35   

TFT_eSPI tft = TFT_eSPI();

// Modes
#define MODE_BANDS 0
#define MODE_CALLSIGN 1
int currentMode = MODE_CALLSIGN; // Default to Mode 2 on boot

// Band storage
String bandLines[5];
int bandCount = 0;
int currentBand = 0;

// Timers
unsigned long lastBandUpdate = 0;
unsigned long lastToggle = 0;
bool showCallsign = true;

// ---- 1. Utility: Remove ANSI codes ----
void removeAnsiCodes(String &s) {
    int esc;
    while ((esc = s.indexOf("\x1B[")) != -1) {
        int end = s.indexOf('m', esc);
        if (end == -1) break;
        s.remove(esc, end - esc + 1);
    }
}

// ---- 2. Header: Mode 1 (Band Info) ----
void drawTimeRow() {
    struct tm timeinfo;
    time_t now = time(nullptr); 
    gmtime_r(&now, &timeinfo);
    char utcTime[6];
    strftime(utcTime, sizeof(utcTime), "%H:%M", &timeinfo);

    time_t istNow = now + (5 * 3600) + (30 * 60);
    localtime_r(&istNow, &timeinfo);
    char istTime[6];
    strftime(istTime, sizeof(istTime), "%H:%M", &timeinfo);

    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(5, 5);
    tft.print("IST "); tft.print(istTime);
    tft.setCursor(130, 5);
    tft.print("UTC "); tft.print(utcTime);
    tft.drawFastHLine(0, 25, 240, TFT_DARKGREY);
}

// ---- 3. Mode 1 logic ----
void updateBandConditions() {
    if (WiFi.status() != WL_CONNECTED) return;
    WiFiClientSecure client; client.setInsecure();
    HTTPClient http;
    http.begin(client, url);
    http.setUserAgent("curl/8.0");

    if (http.GET() == HTTP_CODE_OK) {
        String payload = http.getString();
        removeAnsiCodes(payload);
        payload.replace("\r", "");
        bandCount = 0;
        int start = 0;
        while (true) {
            int end = payload.indexOf('\n', start);
            if (end == -1) break;
            String line = payload.substring(start, end);
            start = end + 1;
            if (line.indexOf("|") != -1 && line.indexOf("m-") != -1) {
                if (bandCount < 5) bandLines[bandCount++] = line;
            }
        }
        if (bandCount > 0) {
            String line = bandLines[currentBand];
            line.replace("|", ""); line.trim();
            int dayIndex = line.indexOf("day:");
            int nightIndex = line.indexOf("night:");
            String bandRange = line.substring(0, dayIndex);
            String dayVal = line.substring(dayIndex + 4, nightIndex);
            String nightVal = line.substring(nightIndex + 6);

            tft.fillScreen(TFT_BLACK);
            drawTimeRow();
            tft.setTextSize(3);
            tft.setTextColor(TFT_RED);
            tft.setCursor(10, 45);
            tft.println(bandRange);
            tft.setTextSize(2);
            tft.setTextColor(TFT_YELLOW);
            tft.setCursor(10, 85);
            tft.print("DAY: "); tft.setTextColor(TFT_WHITE); tft.println(dayVal);
            tft.setCursor(10, 110);
            tft.setTextColor(TFT_GREEN); tft.print("NIGHT: "); tft.setTextColor(TFT_WHITE); tft.println(nightVal);
            currentBand = (currentBand + 1) % bandCount;
        }
    }
    http.end();
}

// ---- 4. Mode 2: Callsign & Time (Yellow BG, Red Bold Border) ----
void drawCallsignScreen() {
    if (millis() - lastToggle > 3000) {
        showCallsign = !showCallsign;
        lastToggle = millis();
        
        tft.fillScreen(TFT_YELLOW);

        // Draw Bold Red Border (3 pixels thick)
        tft.drawRect(0, 0, 240, 135, TFT_RED);
        tft.drawRect(1, 1, 238, 133, TFT_RED);
        tft.drawRect(2, 2, 236, 131, TFT_RED);

        tft.setTextColor(TFT_RED, TFT_YELLOW);

        struct tm timeinfo;
        time_t now = time(nullptr);
        time_t istNow = now + (5 * 3600) + (30 * 60);
        localtime_r(&istNow, &timeinfo);

        if (showCallsign) {
            // Using a scale factor that looks smoother
            tft.setTextDatum(MC_DATUM); // Middle Center
            tft.setTextSize(6); 
            tft.drawString("VU33OM", 120, 67);
        } else {
            char timeStr[10];
            char dateStr[20];
            strftime(timeStr, sizeof(timeStr), "%I:%M", &timeinfo); // Removed %p for larger digits
            strftime(dateStr, sizeof(dateStr), "%d %b %Y", &timeinfo);

            // Large Bold Time
            tft.setTextDatum(MC_DATUM);
            tft.setTextSize(6);
            tft.drawString(timeStr, 120, 50);

            // Bold Date
            tft.setTextSize(3);
            tft.drawString(dateStr, 120, 100);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON1, INPUT_PULLUP);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 40);
    tft.println("Connecting WiFi...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    tft.fillScreen(TFT_BLACK);
}

void loop() {
    static bool lastBtn = HIGH;
    bool btn = digitalRead(BUTTON1);

    if (lastBtn == HIGH && btn == LOW) { 
        currentMode = (currentMode == MODE_BANDS) ? MODE_CALLSIGN : MODE_BANDS;
        tft.fillScreen(TFT_BLACK);
        lastBandUpdate = 0; 
        lastToggle = 0; 
        delay(200); 
    }
    lastBtn = btn;

    if (currentMode == MODE_CALLSIGN) {
        drawCallsignScreen();
    } 
    else {
        if (millis() - lastBandUpdate > 5000) {
            lastBandUpdate = millis();
            updateBandConditions();
        }
    }
}
