#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Servo.h>
#include "config.h"
#include "secrets.h"

// Function declarations
void handleNewMessages(int numNewMessages);
void pressButton();

// WiFi and Telegram
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// Servo
Servo upsServo;

// Timing
unsigned long lastBotCheck = 0;

// State
bool isPressed = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== UPS Controller - Telegram Bot ===");
  
  // Setup pins
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  
  // Attach servo
  upsServo.attach(SERVO_PIN);
  upsServo.write(SERVO_REST_ANGLE);
  Serial.println("✓ Servo initialized at rest position");
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n✓ WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Skip SSL certificate validation
  secured_client.setInsecure();
  
  // Configure SSL certificate (for Telegram)
  // secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  Serial.println("✓ Telegram bot ready");
  Serial.println("Authorized Chat ID: " + String(CHAT_ID));
  Serial.println("\nListening for /ups_ commands...\n");
  
  // LED on = ready
  digitalWrite(STATUS_LED, HIGH);
}

void loop() {
  // Check for new messages every BOT_CHECK_INTERVAL
  if (millis() - lastBotCheck > BOT_CHECK_INTERVAL) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    
    lastBotCheck = millis();
  }
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    
    Serial.println("Message from: " + from_name + " (" + chat_id + ")");
    Serial.println("Text: " + text);
    
    // Security: Only respond to authorized user
    if (chat_id != CHAT_ID) {
      Serial.println("⚠️  Unauthorized user! Ignoring.");
      bot.sendMessage(chat_id, "⛔ Unauthorized access");
      continue;
    }
    
    // Only process commands starting with /ups_
    if (!text.startsWith("/ups_")) {
      Serial.println("Not for UPS controller, ignoring.");
      continue;
    }
    
    // Blink LED to show processing
    digitalWrite(STATUS_LED, LOW);
    delay(100);
    digitalWrite(STATUS_LED, HIGH);
    
    // Process commands
    if (text == "/ups_press") {
      pressButton();
      bot.sendMessage(chat_id, "✅ UPS button pressed!");
      
    } else if (text == "/ups_status") {
      String status = "🔌 UPS Controller Status:\n";
      status += "WiFi: Connected ✓\n";
      status += "IP: " + WiFi.localIP().toString() + "\n";
      status += "Servo: " + String(isPressed ? "Pressed" : "Rest") + "\n";
      bot.sendMessage(chat_id, status);
      
    } else if (text == "/ups_help") {
      String help = "🤖 UPS Controller Commands:\n\n";
      help += "/ups_press - Press UPS button\n";
      help += "/ups_status - Get status\n";
      help += "/ups_help - Show this help";
      bot.sendMessage(chat_id, help);
      
    } else {
      bot.sendMessage(chat_id, "❓ Unknown command. Try /ups_help");
    }
    
    Serial.println("✓ Command processed\n");
  }
}

void pressButton() {
  Serial.println("→ Pressing UPS button...");
  
  isPressed = true;
  upsServo.write(SERVO_PRESS_ANGLE);
  delay(PRESS_DURATION);
  
  upsServo.write(SERVO_REST_ANGLE);
  isPressed = false;
  
  Serial.println("→ Button released");
}
