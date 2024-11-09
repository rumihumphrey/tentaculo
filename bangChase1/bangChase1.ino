#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>
#include <HTTPClient.h>

const char *ssid = "Yurinet";
const char *password = "tatted81528;retransform";

#define PIN 6
#define NUMPIXELS 150

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80);

Adafruit_NeoPixel onePixel = Adafruit_NeoPixel(1, 33, NEO_GRB + NEO_KHZ800);

bool chaseActive = false;

const char *serverUrl = "http://192.168.86.244:5002/chase";

HTTPClient http;
String message;

void setup() {
  //Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  // Initialize the NeoPixel strip
  pixels.begin();
  pixels.clear();
  pixels.show();

  onePixel.begin();           // Start the NeoPixel object
  onePixel.clear();           // Set NeoPixel color to black (0,0,0)
  onePixel.setBrightness(5);  // Affects all subsequent settings
  onePixel.show();

  server.on("/chase", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Chase animation triggered");
    chaseActive = true;
    request->send(200, "text/plain", "Chase animation started");
  });

  server.begin();
}

void loop() {
  if (chaseActive) {
    startChase();
  }
}

void startChase() {
  // Initial colors, delays, and widths for the three animations
  uint32_t colors[] = {
    pixels.Color(random(0, 256), random(0, 256), random(0, 256)),
    pixels.Color(random(0, 256), random(0, 256), random(0, 256)),
    pixels.Color(random(0, 256), random(0, 256), random(0, 256))
  };

  int delays[] = {
    random(10, 12),  // Broader range for more dramatic speed changes
    random(2, 10),
    random(10, 100)
  };

  int widths[] = {
    random(1, 5),  // Random width between 1 and 5
    random(1, 15),
    random(1, 12)
  };

  int positions[] = { 0, NUMPIXELS / 3, 2 * NUMPIXELS / 3 };

  unsigned long lastUpdate[] = { 0, 0, 0 };

  while (chaseActive) {
    for (int i = 0; i < 3; i++) {
      // Check if it's time to update the position for this animation
      if (millis() - lastUpdate[i] >= delays[i]) {
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

          message = "{\"message\":\"Bang\",";
          message += "\"color\":" + String(colors[i]) + ",";
          message += "\"delay\":" + String(delays[i]) + ",";
          message += "\"width\":" + String(widths[i]) + "}";
          http.POST(message);
        }

        lastUpdate[i] = millis();
      }
    }
  }
}
