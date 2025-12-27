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

TFT_eSPI tft = TFT_eSPI();

// Band storage
String bandLines[5];
int bandCount = 0;
int currentBand = 0;

// ---- Remove ANSI color codes ----
void removeAnsiCodes(String &s) {
    int esc;
    while ((esc = s.indexOf("\x1B[")) != -1) {
        int end = s.indexOf('m', esc);
        if (end == -1) break;
        s.remove(esc, end - esc + 1);
    }
}

// ---- Draw IST & UTC Time ----
void drawTimeRow() {
    struct tm timeinfo;
    time_t now = time(nullptr); 

    // UTC
    gmtime_r(&now, &timeinfo);
    char utcTime[6];
    strftime(utcTime, sizeof(utcTime), "%H:%M", &timeinfo);

    // IST (UTC + 5:30)
    time_t istNow = now + (5 * 3600) + (30 * 60);
    localtime_r(&istNow, &timeinfo);
    char istTime[6];
    strftime(istTime, sizeof(istTime), "%H:%M", &timeinfo);

    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    
    tft.setCursor(5, 5);
    tft.print("IST ");
    tft.print(istTime);

    tft.setCursor(130, 5);
    tft.print("UTC ");
    tft.print(utcTime);
    
    tft.drawFastHLine(0, 25, 240, TFT_DARKGREY);
}

void setup() {
    Serial.begin(115200);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 40);
    tft.println("Connecting WiFi...");
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 40);
    tft.println("WiFi Connected!");
    delay(1000);
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure *client = new WiFiClientSecure;
        if(client) {
            client->setInsecure(); // This is the correct way for WiFiClientSecure
            {
                HTTPClient http;
                http.begin(*client, url); // Pass the secure client here
                http.setUserAgent("curl/8.0");
                
                int httpCode = http.GET();

                if (httpCode == HTTP_CODE_OK) {
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
                        line.replace("|", "");
                        line.trim();

                        int dayIndex = line.indexOf("day:");
                        int nightIndex = line.indexOf("night:");

                        String bandRange = line.substring(0, dayIndex);
                        bandRange.trim();
                        String dayVal = line.substring(dayIndex + 4, nightIndex);
                        dayVal.trim();
                        String nightVal = line.substring(nightIndex + 6);
                        nightVal.trim();

                        tft.fillScreen(TFT_BLACK);
                        drawTimeRow();

                        tft.setTextSize(3);
                        tft.setTextColor(TFT_RED);
                        tft.setCursor(10, 40);
                        tft.println(bandRange);

                        tft.setTextSize(2);
                        tft.setTextColor(TFT_YELLOW);
                        tft.setCursor(10, 80);
                        tft.print("DAY:   ");
                        tft.setTextColor(TFT_WHITE);
                        tft.println(dayVal);

                        tft.setTextColor(TFT_GREEN);
                        tft.setCursor(10, 110);
                        tft.print("NIGHT: ");
                        tft.setTextColor(TFT_WHITE);
                        tft.println(nightVal);

                        currentBand++;
                        if (currentBand >= bandCount) currentBand = 0;
                    }
                }
                http.end();
            }
            delete client;
        }
    }
    
    delay(5000); 
}
