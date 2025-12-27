

# File name 001main.cpp
# Solar HF Band Conditions Display (ESP32 + TFT)

This project displays **live HF band propagation conditions** (Day/Night) on a **TFT display** using an **ESP32**, fetching data from **https://solar.w5mmw.net/**.

---

## Features
- Wi-Fi enabled ESP32
- HTTPS data fetch (ANSI codes removed)
- Displays:
  - **IST & UTC time** (24-hour format)
  - **HF band range** (e.g. 80m–40m)
  - **Day & Night propagation status**
- Automatically cycles through up to **5 bands**
- Uses **TFT_eSPI** library

---

## Hardware Required
- ESP32
- TFT display (compatible with TFT_eSPI)
- Wi-Fi access

---

## Libraries Used
- WiFi.h  
- WiFiClientSecure.h  
- HTTPClient.h  
- TFT_eSPI.h  
- time.h  

---

## Setup
1. Edit Wi-Fi credentials in the code:
   ```cpp
   const char* ssid = "YOUR_SSID";
   const char* password = "YOUR_PASSWORD";
   ```

2. Configure **TFT_eSPI** for your display.
3. Upload the sketch to ESP32.

---

## Display Layout
- **Top Row:** IST & UTC Time
- **Middle:** HF Band Name
- **Bottom:** Day and Night signal conditions

---

## Refresh & Rotation
- Data refresh: **Every 5 seconds**
- Bands rotate automatically

---

## Notes
- HTTPS certificate validation is disabled using `setInsecure()`
- Intended for **ham radio / amateur radio monitoring**
