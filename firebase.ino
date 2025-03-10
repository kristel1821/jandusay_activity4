#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

const char* ssid = "KSMJ";
const char* password = "07212005";
const char* firestoreUrl = "https://firestore.googleapis.com/v1/projects/mbc-integ-f5af7/databases/(default)/documents/sd?key=AIzaSyBAdkhM-EwYpiDvzZtx8cUjsSAmX0v5Ivw";

const char* ntpServer = "time.google.com";
const long gmtOffset_sec = 8 * 3600; // GMT+8 for Philippines
const int daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("\nConnected to Wi-Fi");

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", &timeinfo);

    // Prepare HTTP client
    HTTPClient http;
    http.begin(firestoreUrl);
    http.addHeader("Content-Type", "application/json");

    // JSON payload with datetime
    String jsonData = String(R"(
      {
        "fields": {
          "temperature": {"stringValue": "25.5"},
          "humidity": {"stringValue": "60"},
          "light_level": {"stringValue": "21"},
          "timestamp": {"stringValue": ")") + timestamp + R"("}
        }
      }
    )";

    // Send POST request
    int httpResponseCode = http.POST(jsonData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Print response for debugging
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error: " + http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
  }

  delay(5000); // Send data every 5 seconds


}
