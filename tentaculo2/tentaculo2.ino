#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h> // Include the ArduinoJson library

const char *ssid = "Yurinet";
const char *password = "tatted81528;retransform";

#define PIN 6
#define NUMPIXELS 150
#define UDP_PORT 4210

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
WiFiUDP udp;

struct ChaseAnimation {
  uint32_t color;
  int delay;
  int width;
  int position;
  unsigned long lastUpdate;
  bool active;
};

ChaseAnimation animations[10];  // Allow up to 5 simultaneous chase animations

void setup() {
  Serial.begin(115200);  // Initialize serial communication for debugging
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("Connected to WiFi");

  // Initialize the NeoPixel strip
  pixels.begin();
  pixels.clear();
  pixels.show();

  // Initialize the UDP listener
  udp.begin(UDP_PORT);
  Serial.println("Listening for UDP packets...");
  
  // Initialize animations
  for (int i = 0; i < 10; i++) {
    animations[i].active = false;
  }
}

void loop() {
  // Handle incoming UDP messages
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
      Serial.print("Received UDP packet: ");
      Serial.println(incomingPacket);
      handleUdpMessage(String(incomingPacket));
    }
  }

  // Update active animations
  for (int i = 0; i < 10; i++) {
    if (animations[i].active) {
      updateChase(i);
    }
  }
}

void handleUdpMessage(String message) {
  // Parse incoming message
  DynamicJsonDocument doc(1024); // Create a JSON document
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("Failed to parse UDP message: ");
    Serial.println(message);
    return;
  }

  int slot = -1;
  for (int i = 0; i < 10; i++) {
    if (!animations[i].active) {
      slot = i;
      break;
    }
  }

  if (slot == -1) {
    Serial.println("No available slot for new animation");
    return;
  }

  // Assign values to the next available slot
  animations[slot].color = pixels.Color(doc["r"], doc["g"], doc["b"]);
  animations[slot].delay = doc["delay"];
  animations[slot].width = doc["width"];
  animations[slot].position = 0;
  animations[slot].lastUpdate = millis();
  animations[slot].active = true;

  Serial.print("Started new animation in slot ");
  Serial.print(slot);
  Serial.print(" with color: R=");
  Serial.print(doc["r"].as<int>());
  Serial.print(", G=");
  Serial.print(doc["g"].as<int>());
  Serial.print(", B=");
  Serial.print(doc["b"].as<int>());
  Serial.print(", delay: ");
  Serial.print(doc["delay"].as<int>());
  Serial.print(", width: ");
  Serial.println(doc["width"].as<int>());
}

void updateChase(int index) {
  if (millis() - animations[index].lastUpdate >= animations[index].delay) {
    // Clear previous position
    for (int j = 0; j < animations[index].width; j++) {
      pixels.setPixelColor((animations[index].position - j - 1 + NUMPIXELS) % NUMPIXELS, 0);
    }

    // Set current position
    for (int j = 0; j < animations[index].width; j++) {
      pixels.setPixelColor((animations[index].position + j) % NUMPIXELS, animations[index].color);
    }
    pixels.show();

    // Update position
    animations[index].position++;
    animations[index].lastUpdate = millis();

    // Check if the animation has reached the end
    if (animations[index].position >= NUMPIXELS) {
      animations[index].active = false;
      Serial.print("Animation in slot ");
      Serial.print(index);
      Serial.println(" has finished.");
    }
  }
}
