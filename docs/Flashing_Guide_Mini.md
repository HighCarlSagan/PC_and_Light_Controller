ESP32 QUICK FLASH GUIDE - PC & LIGHT CONTROLLER
================================================

HARDWARE SETUP
--------------
ESP32 #1: "UPS"     → Controls servo motor (GPIO 13)
ESP32 #2: "LIGHTS"  → Controls 3 switches (GPIO 25, 26, 27)

Both use:
- WiFi: WIFI_SSID
- Same Telegram bot token
- Same Chat ID: 408698756


FLASH ESP32 #1 (UPS CONTROLLER)
--------------------------------

1. Label board with "UPS" (permanent marker)

2. Connect USB and verify
   lsusb | grep CP210
   ls /dev/ttyUSB*

3. Navigate to firmware
   cd ~/Projects/Hardware/PC_and_Light_Controller/firmware/ups-controller

4. Verify secrets.h exists
   cat include/secrets.h
   (Should show WiFi credentials, bot token, chat ID)

5. Compile and upload
   pio run --target clean
   pio run --target upload
   HOLD BOOT button when you see "Connecting..."

6. Monitor output
   pio device monitor

   Expected output:
   === UPS Controller - Telegram Bot ===
   ✓ Servo initialized
   ✓ WiFi connected!
   IP address: 192.168.0.XX  ← Note this down
   Listening for /ups_ commands...

7. Test from Telegram
   Send: /ups_help
   Should get command list reply

8. Unplug ESP32 #1


FLASH ESP32 #2 (LIGHTS CONTROLLER)
-----------------------------------

1. Label board with "LIGHTS"

2. Connect USB (ESP32 #1 should be unplugged)
   lsusb | grep CP210

3. Navigate to firmware
   cd ~/Projects/Hardware/PC_and_Light_Controller/firmware/lights-controller

4. Verify secrets.h (copy from UPS if missing)
   cat include/secrets.h

5. Compile and upload
   pio run --target clean
   pio run --target upload
   HOLD BOOT when "Connecting..."

6. Monitor output
   pio device monitor

   Expected output:
   === Lights Controller - Telegram Bot ===
   ✓ LED pins initialized
   ✓ WiFi connected!
   IP address: 192.168.0.YY  ← Note this down
   Listening for /lights_ commands...

7. Test from Telegram
   Send: /lights_help
   Should get command list reply


TEST BOTH TOGETHER
------------------

1. Plug in both ESP32s (via USB or 5V power)

2. Test command isolation
   Send: /ups_press      → Only UPS responds
   Send: /lights_1_on    → Only Lights responds

   ✅ Each ESP32 ignores commands not meant for it


TROUBLESHOOTING
---------------

Upload fails (Connecting......):
  Hold BOOT button on ESP32 during upload
  Or: sudo chmod 666 /dev/ttyUSB0

WiFi won't connect:
  Check secrets.h has correct credentials
  Verify WiFi is 2.4GHz (ESP32 doesn't do 5GHz)
  Move ESP32 closer to router

Serial monitor shows garbage:
  pio device monitor -b 115200

Bot not responding:
  Verify Chat ID is 408698756 in secrets.h
  Verify bot token is correct
  Check serial monitor for "Unauthorized user"


QUICK REFERENCE
---------------

Check USB:        lsusb | grep CP210
UPS firmware:     cd ~/Projects/.../firmware/ups-controller
Lights firmware:  cd ~/Projects/.../firmware/lights-controller
Upload:           pio run --target upload (hold BOOT)
Monitor:          pio device monitor (Ctrl+C to exit)


CONFIGURATION RECORD
--------------------

ESP32 #1 (UPS):
  IP Address: _______________
  Commands: /ups_press, /ups_status, /ups_help
  GPIO: 13 (Servo)

ESP32 #2 (Lights):
  IP Address: _______________
  Commands: /lights_1_on, /lights_all_off, /lights_status
  GPIO: 25, 26, 27 (Switches)


WHAT'S NEXT
-----------

1. ✅ Both ESP32s flashed and tested
2. Get servo motor (SG90) and test with /ups_press
3. Get TPS259807 chips for actual switch control
4. Design PCBs in KiCad
5. Order and assemble custom boards