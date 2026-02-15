#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

#define PIN 4
#define LED_COUNT 60
#define MAX_TRAILS 10   // how many simultaneous trails allowed

Adafruit_NeoPixel leds(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

struct Trail {
  int position;
  bool active;
};

Trail trails[MAX_TRAILS];

const char* ssid = "HDE";
const char* password = "hde12345";

WebServer server(80);

void spawnTrail() {
  for (int i = 0; i < MAX_TRAILS; i++) {
    if (!trails[i].active) {
      trails[i].active = true;
      trails[i].position = 0;  // start at beginning
      break;
    }
  }
}

void updateTrails() {
  // Slight fade of entire strip
  for (int i = 0; i < LED_COUNT; i++) {
    uint32_t color = leds.getPixelColor(i);

    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    // fade factor
    r = r * 0.6;
    g = g * 0.6;
    b = b * 0.6;

    leds.setPixelColor(i, r, g, b);
  }

  // Move and draw trails
  for (int i = 0; i < MAX_TRAILS; i++) {
    if (trails[i].active) {
      leds.setPixelColor(trails[i].position, 0, 255, 0);
      trails[i].position++;

      if (trails[i].position >= LED_COUNT) {
        trails[i].active = false;
      }
    }
  }

  leds.show();
}

void handleTrigger() {
  if (server.method() == HTTP_POST) {
    
    String body = server.arg("plain");  // Raw POST body
    Serial.println("Received POST:");
    Serial.println(body);

    // Example action: toggle LED
    spawnTrail();

    // Send JSON response
    server.send(200, "application/json",
      "{\"status\":\"success\",\"message\":\"Triggered\"}");
  } else {
    server.send(405, "application/json",
      "{\"error\":\"Method Not Allowed\"}");
  }
}

void setup() {

  leds.begin();
  leds.clear();
  leds.setBrightness(35);
  leds.show();

  // Initialize trails
  for (int i = 0; i < MAX_TRAILS; i++) {
    trails[i].active = false;
  }

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/trigger", HTTP_POST, handleTrigger);

  server.begin();
  Serial.println("HTTP server started");
}

unsigned long lastTime = 0;

void loop() {
  server.handleClient();
  
  unsigned long delta = millis() - lastTime;
  if(delta > 100) {
    updateTrails();
    lastTime = millis();
  } 
}
