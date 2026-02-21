#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Configuration ---
const char* ssid = "YOURSSID";
const char* password = "YOURPASS";
const char* callsign = "CALLSIGN";
const char* passcode = "20464";
const char* aprs_server = "rotate.aprs2.net";
int aprs_port = 14580;

const char* beacon_packet = "CALLSIGN>APRS,TCPIP*:=2016.23N/07300.12EyESP32 Beacon";

// NTP Setup (IST is UTC + 5.5 hours = 19800 seconds)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient client;

unsigned long lastBeaconTime = 0;
const unsigned long beaconInterval = 10000; // 10 Seconds

void aviationStrobe() {
    digitalWrite(LED_BUILTIN, LOW);  // On
    delay(40);
    digitalWrite(LED_BUILTIN, HIGH); // Off
    delay(80);
    digitalWrite(LED_BUILTIN, LOW);  // On
    delay(40);
    digitalWrite(LED_BUILTIN, HIGH); // Off
}

void sendAPRS() {
    lcd.clear();
    lcd.print("TX APRS BEACON");
    aviationStrobe();

    if (client.connect(aprs_server, aprs_port)) {
        lcd.setCursor(0, 1);
        String login = "user " + String(callsign) + " pass " + String(passcode) + " vers ESP8266_IST 1.3\n";
        client.print(login);
        
        for(int i=0; i<8; i++) { lcd.print("."); delay(150); }

        client.println(beacon_packet);
        delay(200);
        client.stop();
        
        lcd.setCursor(0, 1);
        lcd.print("SUCCESS!        ");
        delay(1500);
    } else {
        lcd.setCursor(0, 1);
        lcd.print("SERVER FAIL     ");
        delay(3000);
    }
}

void updateLCD() {
    timeClient.update();
    
    // Line 1: Real Time
    lcd.setCursor(0, 0);
    lcd.print("IST: ");
    lcd.print(timeClient.getFormattedTime());
    lcd.print("      "); // Clear end of line

    // Line 2: Uptime
    unsigned long up = millis() / 1000;
    int h = up / 3600;
    int m = (up % 3600) / 60;
    int s = up % 60;
    
    char upBuf[17];
    sprintf(upBuf, "UP: %02dh %02dm %02ds", h, m, s);
    lcd.setCursor(0, 1);
    lcd.print(upBuf);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    Wire.begin(4, 5);
    lcd.init();
    lcd.backlight();
    
    lcd.print("Connecting WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
    
    timeClient.begin();
    lcd.clear();
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastBeaconTime >= beaconInterval || lastBeaconTime == 0) {
        sendAPRS();
        lastBeaconTime = millis();
        lcd.clear();
    } else {
        updateLCD();
        // Strobe every 3 seconds for that aviation look
        if (currentMillis % 3000 < 50) { aviationStrobe(); }
    }
}
