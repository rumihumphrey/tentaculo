#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>

const char *ssid = "Yurinet";
const char *password = "tatted81528;retransform";

#define PIN 6
#define NUMPIXELS 150
#define UDP_PORT 4210

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
WiFiUDP udp;

Adafruit_NeoPixel onePixel = Adafruit_NeoPixel(1, 33, NEO_GRB + NEO_KHZ800);

bool chaseActive = false;

const char *udpAddress = "192.168.86.250";

String message;
unsigned long lastUpdate[3] = { 0, 0, 0 }; // Track the last update time for each animation
int positions[] = { 0, NUMPIXELS / 3, 2 * NUMPIXELS / 3 };
uint32_t colors[3];
int delays[3];
int widths[3];
bool initialUpdateSent[3] = { false, false, false }; // Track if the initial update has been sent for each segment

void setup() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Initialize the NeoPixel strip
  pixels.begin();
  pixels.clear();
  pixels.show();

  onePixel.begin();           // Start the NeoPixel object
  onePixel.clear();           // Set NeoPixel color to black (0,0,0)
  onePixel.setBrightness(5);  // Affects all subsequent settings
  onePixel.show();

  // Initialize colors, delays, and widths for animations
  for (int i = 0; i < 3; i++) {
    colors[i] = pixels.Color(random(0, 256), random(0, 256), random(0, 256));
    delays[i] = random(10, 30);
    widths[i] = random(1, 7);
  }

  // Start chase animation when the system starts
  chaseActive = true;
}

void loop() {
  if (chaseActive) {
    updateChase();
  }
}

void updateChase() {
  for (int i = 0; i < 3; i++) {
    // Send the update when the chase starts (only once)
    if (!initialUpdateSent[i]) {
      sendChaseUpdate(i, true);
      initialUpdateSent[i] = true; // Ensure this update is sent only once
    }

    if (millis() - lastUpdate[i] >= delays[i]) {
      // Send the update slightly before the animation reaches the end to compensate for delay
      if (positions[i] == NUMPIXELS - 5) {
        sendChaseUpdate(i, false);
      }

      // Turn off the previous pixels
      for (int j = 0; j < widths[i]; j++) {
        pixels.setPixelColor((positions[i] - j - 1 + NUMPIXELS) % NUMPIXELS, 0);
      }

      // Set the current block of pixels to the color
      for (int j = 0; j < widths[i]; j++) {
        pixels.setPixelColor((positions[i] + j) % NUMPIXELS, colors[i]);
      }
      pixels.show();

      // Move the position
      positions[i] = (positions[i] + 1) % NUMPIXELS;

      // Check if the color reached the end of the strip
      if (positions[i] == 0) {
        // Generate a new color, delay, and width for the next cycle
        colors[i] = pixels.Color(random(0, 256), random(0, 256), random(0, 256));
        delays[i] = random(10, 30);
        widths[i] = random(1, 7);
        initialUpdateSent[i] = false; // Reset the flag to allow a new start update in the next cycle
      }

      lastUpdate[i] = millis();
    }
  }
}

void sendChaseUpdate(int index, bool isStart) {
  // Send the update indicating start or end of the chase animation
  message = "{\"message\":\"";
  message += (isStart ? "start" : "end");
  message += "\",";
  message += "\"color\":" + String(colors[index]) + ",";
  message += "\"delay\":" + String(delays[index]) + ",";
  message += "\"width\":" + String(widths[index]) + "}";

  udp.beginPacket(udpAddress, UDP_PORT);
  udp.write((const uint8_t *)message.c_str(), message.length());
  udp.endPacket();
}
