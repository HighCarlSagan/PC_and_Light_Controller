# PC & Light Controller

ESP32-based remote control system for PC power management and room lighting via Telegram Bot.

![Board Render](docs/images/board_render.png)

## Features

- **3x TPS259807ONRGER eFuse Switches**: DC load switching for IKEA LED lights (5V)
- **Servo Motor Control**: Automated UPS power button actuation (SG90/MG90S)
- **Remote Access**: Telegram Bot integration for secure remote control from anywhere
- **Dual ESP32 Design**: Separate controllers for UPS and lighting
- **Custom PCB**: 4-layer boards designed in KiCad (planned)
- **Power**: Powered by custom 60W dual-rail PSU (3.3V/5V @ 6A each)

## Current Setup

### Hardware
- **UPS Controller ESP32**: Controls servo motor for UPS button (GPIO 13)
- **Lights Controller ESP32**: Controls 3 LED switches (GPIO 25, 26, 27)

### Network
- WiFi: Peela_Ghar
- UPS Controller IP: 192.168.0.84
- Lights Controller IP: 192.168.0.84 (when testing on same board)

### Telegram Bot
- Single bot controls both ESP32 devices
- Command prefixes: `/ups_*` for UPS, `/lights_*` for lights
- Commands: See [docs/commands/TELEGRAM_COMMANDS.md](docs/commands/TELEGRAM_COMMANDS.md)
- Works from anywhere via Telegram (no port forwarding needed)
- Security: Chat ID verification (only authorized user can control)

## Quick Start

### Using the Controllers

1. **Power on ESP32s** (via USB or 60W PSU when available)
2. **Open Telegram** on your phone
3. **Send commands** to your bot:
   - `/ups_press` - Press UPS button
   - `/ups_status` - Get UPS controller status
   - `/lights_1_on` - Turn on light 1
   - `/lights_all_off` - Turn all lights off
   - `/lights_status` - Check all lights status

### Flashing Firmware

**UPS Controller:**
```bash
cd firmware/ups-controller
pio run --target upload
pio device monitor
```

**Lights Controller:**
```bash
cd firmware/lights-controller
pio run --target upload
pio device monitor
```

**Note:** Hold BOOT button on ESP32 when you see "Connecting..." during upload.

### First Time Setup

1. **Create Telegram Bot**
   - Message @BotFather on Telegram
   - Send `/newbot` and follow instructions
   - Save the bot token

2. **Get Your Chat ID**
   - Message @userinfobot on Telegram
   - Note your Chat ID

3. **Configure Secrets**
```bash
   cd firmware/ups-controller/include
   cp secrets.h.example secrets.h
   nano secrets.h
   # Add your WiFi credentials, bot token, and Chat ID
```

4. **Flash Both ESP32s** (see Flashing Firmware above)

## Hardware Specifications

### Main Components

| Component | Specification | Purpose |
|-----------|---------------|---------|
| MCU | ESP32 WiFi Module (2x) | Main controllers |
| Load Switches | 3x TPS259807ONRGER eFuse | LED switching (6A max each) |
| Servo Motor | SG90 or MG90S | UPS button actuation |
| Power Supply | 60W Dual-Rail PSU (5V/3.3V @ 6A) | System power |
| PCB | 4-layer (planned) | Custom board |

### Pinout

**UPS Controller ESP32:**
```
GPIO 2  : Built-in LED (status indicator)
GPIO 13 : Servo PWM signal
VIN     : 5V from PSU
GND     : Common ground
```

**Lights Controller ESP32:**
```
GPIO 2  : Built-in LED (status indicator)
GPIO 25 : TPS259807 #1 Enable (LED switch 1)
GPIO 26 : TPS259807 #2 Enable (LED switch 2)
GPIO 27 : TPS259807 #3 Enable (LED switch 3)
VIN     : 5V from PSU
GND     : Common ground
```

### Schematic

[Download PDF](hardware/exports/schematic.pdf) (Coming soon)

## Firmware Architecture

### Technology Stack

- **Framework**: Arduino (via PlatformIO)
- **Networking**: WiFi + Telegram Bot API
- **Libraries**:
  - WiFi.h (ESP32 WiFi)
  - WiFiClientSecure.h (HTTPS for Telegram)
  - UniversalTelegramBot (Telegram API wrapper)
  - ESP32Servo (Servo motor control)
  - ArduinoJson (JSON parsing)

### Command Interface

Both ESP32s poll the same Telegram bot every 1 second. Each responds only to its command prefix:

**UPS Controller Commands:**
- `/ups_press` - Press UPS power button
- `/ups_status` - Get controller status
- `/ups_help` - Show help

**Lights Controller Commands:**
- `/lights_1_on`, `/lights_1_off` - Control light 1
- `/lights_2_on`, `/lights_2_off` - Control light 2
- `/lights_3_on`, `/lights_3_off` - Control light 3
- `/lights_all_on`, `/lights_all_off` - Control all lights
- `/lights_status` - Get all lights status
- `/lights_help` - Show help

See [TELEGRAM_COMMANDS.md](docs/commands/TELEGRAM_COMMANDS.md) for complete reference.

## Testing Status

**Firmware:** ✅ Complete and Tested
- ✅ WiFi connection
- ✅ Telegram bot communication (both ESP32s)
- ✅ Command parsing and execution
- ✅ Security (Chat ID verification)
- ✅ Command prefix isolation
- ✅ Remote control from anywhere via Telegram

**Hardware:** 🔄 In Progress
- ⏳ Awaiting servo motor (SG90)
- ⏳ Awaiting TPS259807 chips
- ⏳ PCB design (KiCad schematic)
- ⏳ PCB manufacturing
- ⏳ Final assembly

## PCB Manufacturing

### Gerber Files
Gerbers for manufacturing will be in [hardware/exports/gerbers/](hardware/exports/gerbers/) (Coming soon)

### BOM
See [hardware/bom/bom.csv](hardware/bom/bom.csv) for complete parts list (Coming soon)

### Recommended Manufacturers
- JLCPCB (4-layer, ENIG finish recommended)
- PCBWay

## Development Workflow

### KiCad → Gerbers
```bash
cd hardware/kicad
# Design in KiCad
# File → Plot → Generate Gerbers
# Move to hardware/exports/gerbers/
```

### Code → Flash
```bash
cd firmware/ups-controller  # or lights-controller
pio run --target upload
pio device monitor
```

### Update Documentation
```bash
# Take photos of assembled board
# Place in docs/images/
# Update README.md
git add .
git commit -m "docs: add assembly photos"
git push
```

## Project Status

**Software/Firmware:**
- [x] Telegram bot integration
- [x] UPS controller firmware
- [x] Lights controller firmware
- [x] Command parsing and security
- [x] Testing and validation
- [x] Documentation

**Hardware:**
- [ ] Schematic design (KiCad)
- [ ] PCB layout
- [ ] Order components
- [ ] Order PCBs
- [ ] Assembly
- [ ] Hardware testing
- [ ] Enclosure design (FreeCAD)

## Repository Structure
```
PC_and_Light_Controller/
├── firmware/
│   ├── ups-controller/        # UPS ESP32 firmware
│   │   ├── src/main.cpp
│   │   ├── include/
│   │   │   ├── config.h
│   │   │   ├── secrets.h         (gitignored)
│   │   │   └── secrets.h.example
│   │   └── platformio.ini
│   └── lights-controller/     # Lights ESP32 firmware
│       ├── src/main.cpp
│       ├── include/
│       │   ├── config.h
│       │   ├── secrets.h         (gitignored)
│       │   └── secrets.h.example
│       └── platformio.ini
├── hardware/
│   ├── kicad/                 # KiCad schematic & PCB
│   ├── exports/               # Gerbers, PDFs, renders
│   └── bom/                   # Bill of materials
├── docs/
│   ├── commands/
│   │   └── TELEGRAM_COMMANDS.md
│   └── images/
├── .gitignore
├── LICENSE
└── README.md
```

## Photos

*Coming soon after hardware assembly*

## Security

- **Chat ID Verification**: Only authorized Telegram user can send commands
- **No Port Forwarding**: Telegram handles all networking securely
- **HTTPS**: All communication with Telegram uses TLS encryption
- **Secrets Management**: Credentials stored in gitignored `secrets.h` files

## License

- **Hardware**: [CERN-OHL-P v2](LICENSE)
- **Firmware**: MIT License

## Contributing

This is a personal project, but feel free to open issues or submit PRs if you find bugs or have suggestions.

## Author

**Mak** - [HighCarlSagan](https://github.com/HighCarlSagan)

Part of my home automation & hardware design projects. Check out my other repos:
- [Universal 60W PSU](https://github.com/HighCarlSagan/60w-dual-rail-psu)
- [Satellite Power Sizing Tool](...)

---

**⚠️ Safety Note**: This system interfaces with mains-powered equipment (UPS). Ensure proper isolation and safety measures during installation.
