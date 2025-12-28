#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "config.h"
#include "secrets.h"

// Function declarations
void handleNewMessages(int numNewMessages);
void setLED(int channel, bool state);

// WiFi and Telegram
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// Timing
unsigned long lastBotCheck = 0;

// LED States
bool led1State = false;
bool led2State = false;
bool led3State = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== Lights Controller - Telegram Bot ===");
  
  // Setup pins
  pinMode(STATUS_LED, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  
  // All off initially
  digitalWrite(STATUS_LED, LOW);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  
  Serial.println("✓ LED pins initialized");
  
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
  
  Serial.println("✓ Telegram bot ready");
  Serial.println("Authorized Chat ID: " + String(CHAT_ID));
  Serial.println("\nListening for /lights_ commands...\n");
  
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
    
    // Only process commands starting with /lights_
    if (!text.startsWith("/lights_")) {
      Serial.println("Not for Lights controller, ignoring.");
      continue;
    }
    
    // Blink LED to show processing
    digitalWrite(STATUS_LED, LOW);
    delay(100);
    digitalWrite(STATUS_LED, HIGH);
    
    // Process commands
    if (text == "/lights_1_on") {
      setLED(1, true);
      bot.sendMessage(chat_id, "✅ Light 1 ON");
      
    } else if (text == "/lights_1_off") {
      setLED(1, false);
      bot.sendMessage(chat_id, "✅ Light 1 OFF");
      
    } else if (text == "/lights_2_on") {
      setLED(2, true);
      bot.sendMessage(chat_id, "✅ Light 2 ON");
      
    } else if (text == "/lights_2_off") {
      setLED(2, false);
      bot.sendMessage(chat_id, "✅ Light 2 OFF");
      
    } else if (text == "/lights_3_on") {
      setLED(3, true);
      bot.sendMessage(chat_id, "✅ Light 3 ON");
      
    } else if (text == "/lights_3_off") {
      setLED(3, false);
      bot.sendMessage(chat_id, "✅ Light 3 OFF");
      
    } else if (text == "/lights_all_on") {
      setLED(1, true);
      setLED(2, true);
      setLED(3, true);
      bot.sendMessage(chat_id, "✅ All lights ON");
      
    } else if (text == "/lights_all_off") {
      setLED(1, false);
      setLED(2, false);
      setLED(3, false);
      bot.sendMessage(chat_id, "✅ All lights OFF");
      
    } else if (text == "/lights_status") {
      String status = "💡 Lights Status:\n";
      status += "Light 1: " + String(led1State ? "ON ✓" : "OFF") + "\n";
      status += "Light 2: " + String(led2State ? "ON ✓" : "OFF") + "\n";
      status += "Light 3: " + String(led3State ? "ON ✓" : "OFF") + "\n";
      status += "WiFi: Connected ✓\n";
      status += "IP: " + WiFi.localIP().toString();
      bot.sendMessage(chat_id, status);
      
    } else if (text == "/lights_help") {
      String help = "💡 Lights Controller Commands:\n\n";
      help += "/lights_1_on - Turn light 1 ON\n";
      help += "/lights_1_off - Turn light 1 OFF\n";
      help += "/lights_2_on - Turn light 2 ON\n";
      help += "/lights_2_off - Turn light 2 OFF\n";
      help += "/lights_3_on - Turn light 3 ON\n";
      help += "/lights_3_off - Turn light 3 OFF\n";
      help += "/lights_all_on - All lights ON\n";
      help += "/lights_all_off - All lights OFF\n";
      help += "/lights_status - Get status\n";
      help += "/lights_help - Show this help";
      bot.sendMessage(chat_id, help);
      
    } else {
      bot.sendMessage(chat_id, "❓ Unknown command. Try /lights_help");
    }
    
    Serial.println("✓ Command processed\n");
  }
}

void setLED(int channel, bool state) {
  int pin;
  const char* name;
  
  switch(channel) {
    case 1:
      pin = LED1_PIN;
      led1State = state;
      name = "Light 1";
      break;
    case 2:
      pin = LED2_PIN;
      led2State = state;
      name = "Light 2";
      break;
    case 3:
      pin = LED3_PIN;
      led3State = state;
      name = "Light 3";
      break;
    default:
      return;
  }
  
  digitalWrite(pin, state ? HIGH : LOW);
  Serial.print("→ ");
  Serial.print(name);
  Serial.println(state ? " ON" : " OFF");
}
