# 🕒 Word Clock

A custom-built word clock using an ESP32 and addressable LEDs. Displays the current time using illuminated words instead of digits.

---

## 📁 Repository Structure
WORD-CLOCK/
├── cad/                     # CAD models for the enclosure
│   ├── Word Clock Solidworks.zip
│   └── Word Clock.STEP
├── code/                    # Arduino code for the microcontroller
│   └── Word Clock.ino
├── LICENSE
└── README.md

---

## 🔧 Features

- Displays time using words like “It is ten past three”
- Uses WS2812 (NeoPixel) LEDs
- Runs on an ESP32 microcontroller
- Custom-designed 3D-printed/laser-cut enclosure

---

## 📷 Project Preview

*Add a photo or GIF of your working word clock here if available.*

---

## 📦 CAD and Enclosure

Located in the `cad/` folder:
- `Word Clock Solidworks.zip`: Full SolidWorks project
- `Word Clock.STEP`: Universal 3D model for printing or editing

---

## 💡 Electronics

- **Microcontroller**: ESP32  
- **LEDs**: WS2812b (addressable RGB)  
- **Power Supply**: 5V (sized to your LED count)  
- **Optional**: RTC module for timekeeping accuracy

---

## 📜 Arduino Code

Located in the `code/` folder:
- `Word Clock.ino`: Main program written for the Arduino IDE. Handles time conversion to words, LED control, and animations.

> **Dependencies**:  
> - `FastLED` or `NeoPixelBus` library  
> - `NTPClient` and `WiFi` libraries (if using network time)  
> - Adjust LED pin, matrix layout, and time logic as needed

---

## 🚀 Getting Started

1. Install the Arduino IDE  
2. Install required libraries  
3. Open `Word Clock.ino`  
4. Configure your WiFi credentials (if applicable)  
5. Upload to your ESP32  
6. Power your LEDs and enjoy!

---

## 📜 License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.

---

## 🙋‍♂️ Author

**Alfonso Custodio**  
If you have questions, feel free to reach out!
