# 🧭 TrueHomeFinder: The Zero-Waste Navigation Engine

> An over-engineered, satirical navigation console that cuts out unnecessary travel by guiding you in a strict square directly back to where you started.

---

### 👥 Team Details
Team Lead: Abhinadh Shine - Saintgits College of Engineering
Member 2: Ron Geo Roy - Saintgits College of Engineering

---

### 💡 The Problem (That Nobody Had)
Most modern journeys end with people simply wanting to go back home, yet navigation systems insist on routing them to destinations across town first. Traditional pathfinding wastes time, gas, and cognitive energy on intermediate stops.

### ⚙️ The "Solution"
**TrueHomeFinder** cuts out the middleman entirely:
1. The user picks any target destination on earth via a laptop dispatch terminal .
2. The Arduino Uno runs quantum route calculations on an SH1106 OLED HUD.
3. The system directs the user through four strict 90-degree right turns and fixed-pace walking intervals confirmed via tactile hardware button presses.
4. The user arrives back at their exact origin with **100% positional accuracy** and **0% wasted travel**.

---

### 🛠️ Hardware & Tools Used
* **Microcontroller:** Arduino Uno 
* **HUD Display:** 1.3" / 0.96" I2C OLED (SH1106 Driver)
* **Audio Module:** 5V Piezo Buzzer
* **Input:** Tactile momentary push button
* **Software:** Arduino C++, Python 3 (`pyserial`), Adafruit GFX & SH110X libraries

---

### 🔌 Pin Mapping

| Component | Pin | Arduino Uno |
| :--- | :--- | :--- |
| **SH1106 OLED** | VCC / GND | `5V` / `GND` |
| | SCK / SCL | `A5` |
| | SDA | `A4` |
| **Tactile Button** | Leg 1 / Leg 2 | `Pin 2` / `GND` |
| **Piezo Buzzer** | (+) / (-) | `Pin 8` / `GND` |

---

### 🚀 Setup & Execution

#### 1. Flash Firmware
1. Open `src/firmware/firmware.ino` in Arduino IDE.
2. Install libraries: `Adafruit GFX` and `Adafruit_SH110X`.
3. Select board `Arduino Uno` and flash the board.

#### 2. Launch Host Terminal
1. Close the Arduino Serial Monitor so the port is free.
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
