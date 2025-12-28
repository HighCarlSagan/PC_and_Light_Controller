# Telegram Bot Commands

This document lists all available commands for the PC & Light Controller Telegram bot.

## Bot Information
- **Bot Name:** [Your bot name]
- **Bot Username:** @[your_bot_username]
- **Security:** Only responds to authorized Chat ID

## UPS Controller Commands

Control the UPS power button via servo motor.

| Command | Description |
|---------|-------------|
| `/ups_press` | Press the UPS power button (servo activates for 500ms) |
| `/ups_status` | Get UPS controller status (WiFi, IP, servo state) |
| `/ups_help` | Show UPS controller help |

**Example Usage:**
```
/ups_press
```
Response: `✅ UPS button pressed!`

---

## Lights Controller Commands

Control 3 LED light switches.

### Individual Light Control

| Command | Description |
|---------|-------------|
| `/lights_1_on` | Turn Light 1 ON |
| `/lights_1_off` | Turn Light 1 OFF |
| `/lights_2_on` | Turn Light 2 ON |
| `/lights_2_off` | Turn Light 2 OFF |
| `/lights_3_on` | Turn Light 3 ON |
| `/lights_3_off` | Turn Light 3 OFF |

### Bulk Control

| Command | Description |
|---------|-------------|
| `/lights_all_on` | Turn all lights ON |
| `/lights_all_off` | Turn all lights OFF |

### Status & Help

| Command | Description |
|---------|-------------|
| `/lights_status` | Get status of all lights + WiFi info |
| `/lights_help` | Show lights controller help |

**Example Usage:**
```
/lights_1_on
```
Response: `✅ Light 1 ON`
```
/lights_status
```
Response:
```
💡 Lights Status:
Light 1: ON ✓
Light 2: OFF
Light 3: OFF
WiFi: Connected ✓
IP: 192.168.0.84
```

---

## How It Works

### Architecture
- **One Telegram Bot** controls both ESP32 devices
- **Command Prefixes** determine which ESP32 responds:
  - `/ups_*` → UPS Controller ESP32
  - `/lights_*` → Lights Controller ESP32
- Each ESP32 ignores commands not meant for it

### Security
- Only authorized Chat ID can send commands
- Unauthorized users receive "⛔ Unauthorized access" message
- All commands are logged to serial monitor

### Response Time
- Commands are checked every 1 second (polling interval)
- Typical response time: 1-2 seconds

---

## Troubleshooting

**Bot not responding:**
1. Check ESP32 is powered on
2. Check WiFi connection (LED should be solid ON)
3. Check serial monitor for errors
4. Verify bot token and chat ID in `secrets.h`

**Wrong device responding:**
- Check command prefix (`/ups_` vs `/lights_`)
- Each ESP32 only responds to its prefix

**Unauthorized access message:**
- Verify your Chat ID matches the one in `secrets.h`
- Get your Chat ID from @userinfobot

---

## Adding More Devices

To add another ESP32 device:

1. Choose a new command prefix (e.g., `/garage_`)
2. Create new firmware folder
3. Use same bot token and chat ID
4. Check for prefix in `handleNewMessages()`

Example for garage door:
```cpp
if (!text.startsWith("/garage_")) {
  return; // Ignore
}
```

---

## Notes

- Both ESP32s use the **same WiFi network**
- Both ESP32s use the **same Telegram bot**
- Commands work from anywhere in the world (via Telegram)
- No port forwarding required
- No separate server needed
