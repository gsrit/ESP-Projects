# Project 1 - Solar HF Band & Station Display (ESP32 + TFT)
# File Name: 001main.cpp

This project is a dual-mode information dashboard designed for the **LilyGO TTGO T-Display v1.1**. It provides real-time HF propagation data and a high-visibility station identification clock.

---

## 🚀 Key Features

- **Dual-Mode Logic:** Toggle between band conditions and station ID using the onboard button (GPIO 35).
- **Default Boot Mode:** Starts automatically in **Mode 2** (Station ID/Clock).
- **Mode 1: Band Conditions:**
  - Fetches live data from `https://solar.w5mmw.net/`.
  - Displays **IST & UTC** headers.
  - Automatically cycles through 5 HF bands every 5 seconds.
- **Mode 2: Station ID & 12H Clock:**
  - **Callsign:** "VU33OM" displayed in a bold, large font.
  - **Clock:** 12-hour format IST time with a full date.
  - **Visuals:** Bright Yellow background with Red bold text and a **triple-thick Red border**.
  - **Toggling:** Smoothly alternates between callsign and time every 3 seconds.

---

## 🛠 Hardware Specifications

- **Controller:** ESP32 (LilyGO TTGO T-Display v1.1)
- **Display:** 1.14" ST7789 LCD (240x135 resolution)
- **Input:** Onboard Button (Right Side - GPIO 35)
- **Connectivity:** 2.4GHz Wi-Fi for NTP and Data Fetching

---

## 📚 Required Libraries

- `TFT_eSPI` (Optimized for ST7789)
- `WiFi` & `WiFiClientSecure`
- `HTTPClient`
- `time.h`

---

## ⚙️ Setup & Installation

1. **WiFi Configuration:**
   Update the following lines in `001main.cpp`:
   ```cpp
   const char* ssid = "YOUR_SSID";
   const char* password = "YOUR_PASSWORD";

## Some pics of the working ESP

1. **This is mode 2, it shows two output changes every 5 seconds(Default Mode)**

<p align="center">
  <img src="https://github.com/user-attachments/assets/011de4e3-77f6-4560-9d8b-be9555588683"
       width="450"
       style="border: 4px solid #ffffff; margin: 10px;" />
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/8b8c1470-9f1d-4a5f-8201-dff4b427ded3"
       width="450"
       style="border: 4px solid #ffffff; margin: 10px;" />
</p>

1. **This is Mode 1 , It shows all bands one by one every 3 seconds (Needs Button to be pushed for change)**


<p align="center">
  <img src="https://github.com/user-attachments/assets/cf6e820e-9c52-4834-8f3c-8e527c5a3ad2"
       width="450"
       style="border: 4px solid #ffffff; margin: 10px;" />
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/e93aacdc-f2fc-47f2-a4cf-d67faa45c805"
       width="450"
       style="border: 4px solid #ffffff; margin: 10px;" />
</p>



