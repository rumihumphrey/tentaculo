#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h> // Include the ArduinoJson library

#define PIN 6
#define NUMPIXELS 190

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Structure to store chase animations
struct ChaseAnimation {
  uint32_t color;
  int delay;
  int width;
  int position;
  unsigned long lastUpdate;
  bool active;
  bool reverse;
};

// Allow up to 10 simultaneous chase animations
ChaseAnimation animations[10];

void setup() {
  Serial.begin(115200);  // Initialize serial communication

  // Initialize the NeoPixel strip
  pixels.begin();
  pixels.clear();
  pixels.show();

  // Initialize animations
  for (int i = 0; i < 10; i++) {
    animations[i].active = false;
  }
}

void loop() {
  // Handle incoming serial messages
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    handleSerialMessage(message);
  }

  // Update active animations
  for (int i = 0; i < 10; i++) {
    if (animations[i].active) {
      updateChase(i);
    }
  }
}

void handleSerialMessage(String message) {
  // Parse incoming message
  DynamicJsonDocument doc(1024); // Create a JSON document
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    // Serial.print("Failed to parse serial message: ");
    // Serial.println(message);
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
    // Serial.println("No available slot for new animation");
    return;
  }

  // Assign values to the next available slot
  animations[slot].color = pixels.Color(doc["r"], doc["g"], doc["b"]);
  animations[slot].delay = doc["delay"];
  animations[slot].width = doc["width"];
  animations[slot].position = doc["reverse"].as<bool>() ? NUMPIXELS - 1 : 0;
  animations[slot].lastUpdate = millis();
  animations[slot].active = true;
  animations[slot].reverse = doc.containsKey("reverse") ? doc["reverse"].as<bool>() : false;

  // Serial.print("Started new animation in slot ");
  // Serial.print(slot);
  // Serial.print(" with color: R=");
  // Serial.print(doc["r"].as<int>());
  // Serial.print(", G=");
  // Serial.print(doc["g"].as<int>());
  // Serial.print(", B=");
  // Serial.print(doc["b"].as<int>());
  // Serial.print(", delay: ");
  // Serial.print(doc["delay"].as<int>());
  // Serial.print(", width: ");
  // Serial.print(doc["width"].as<int>());
  // Serial.print(", reverse: ");
  // Serial.println(animations[slot].reverse ? "true" : "false");
}

void updateChase(int index) {
  if (millis() - animations[index].lastUpdate >= animations[index].delay) {
    // Clear previous position
    for (int j = 0; j < animations[index].width; j++) {
      int clearPos = animations[index].reverse ? (animations[index].position + j + NUMPIXELS) % NUMPIXELS : (animations[index].position - j + NUMPIXELS) % NUMPIXELS;
      pixels.setPixelColor(clearPos, 0);
    }

    // Set current position
    for (int j = 0; j < animations[index].width; j++) {
      int setPos = animations[index].reverse ? (animations[index].position - j + NUMPIXELS) % NUMPIXELS : (animations[index].position + j) % NUMPIXELS;
      pixels.setPixelColor(setPos, animations[index].color);
    }
    pixels.show();

    // Update position
    if (animations[index].reverse) {
      animations[index].position--;
      if (animations[index].position < -(animations[index].width - 1)) {
        // Clear remaining LEDs and deactivate animation
        clearAnimation(index);
      }
    } else {
      animations[index].position++;
      if (animations[index].position >= NUMPIXELS - animations[index].width + 1) {
        // Clear remaining LEDs and deactivate animation
        clearAnimation(index);
      }
    }

    animations[index].lastUpdate = millis();
  }
}

void clearAnimation(int index) {
  // Clear all LEDs associated with this animation
  for (int j = 0; j < animations[index].width; j++) {
    int clearPos = animations[index].reverse ? (animations[index].position + j + NUMPIXELS) % NUMPIXELS : (animations[index].position - j + NUMPIXELS) % NUMPIXELS;
    pixels.setPixelColor(clearPos, 0);
  }
  pixels.show();
  animations[index].active = false;
  animations[index].position = 0; // Reset position to prevent leftover LEDs
  // Serial.print("Animation in slot ");
  // Serial.print(index);
  // Serial.println(" has finished.");
}
