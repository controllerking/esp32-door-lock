#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// WiFi credentials. yes. i know hardcoded wifi is bad but literally shut up i dont care.
const char* ssid = "SSID_HERE";
const char* password = "PASSWORD_HERE";

// Define GPIO pins for relays and button
const int relay1Pin = 5;  // SPDT Relay 1 for direction
const int relay2Pin = 18; // SPDT Relay 2 for direction
const int relay3Pin = 19; // Relay for power control
const int ledPin = 2;     // Built-in LED for status indication

WebServer server(80);
Preferences preferences;

// Default times for lock/unlock (in milliseconds)
unsigned int lockTime = 2100;
unsigned int unlockTime = 5300;
unsigned int lockReverseTime = 1900;
unsigned int unlockReverseTime = 4900;

void setup() {
  Serial.begin(115200);
  
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(relay3Pin, HIGH); // Keep the power relay off initially

  // Initialize preferences
  preferences.begin("door-lock", false);
  
  // Load saved times
  lockTime = preferences.getUInt("lockTime", lockTime);
  unlockTime = preferences.getUInt("unlockTime", unlockTime);
  lockReverseTime = preferences.getUInt("lockReverseTime", lockReverseTime);
  unlockReverseTime = preferences.getUInt("unlockReverseTime", unlockReverseTime);

  setupWiFi(); // Connect to WiFi
  setupServer(); // Setup web server routes
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupServer() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", generateMainPage());
  });

  server.on("/lock", HTTP_GET, []() {
    lockDoor();
    server.send(200, "text/plain", "Door locking");
  });

  server.on("/unlock", HTTP_GET, []() {
    unlockDoor();
    server.send(200, "text/plain", "Door unlocking");
  });

  server.on("/stop", HTTP_GET, []() {
    stopDoor();
    server.send(200, "text/plain", "Door movement stopped");
  });

  server.on("/settings", HTTP_GET, []() {
    server.send(200, "text/html", generateSettingsPage());
  });

  server.on("/setTimes", HTTP_POST, []() {
    if (server.hasArg("lockTime") && server.hasArg("unlockTime") && server.hasArg("lockReverseTime") && server.hasArg("unlockReverseTime")) {
      lockTime = server.arg("lockTime").toInt();
      unlockTime = server.arg("unlockTime").toInt();
      lockReverseTime = server.arg("lockReverseTime").toInt();
      unlockReverseTime = server.arg("unlockReverseTime").toInt();
      
      // Save the times in preferences
      preferences.putUInt("lockTime", lockTime);
      preferences.putUInt("unlockTime", unlockTime);
      preferences.putUInt("lockReverseTime", lockReverseTime);
      preferences.putUInt("unlockReverseTime", unlockReverseTime);

      server.send(200, "text/plain", "Times updated");
    } else {
      server.send(400, "text/plain", "Invalid parameters");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}

String generateMainPage() {
  String page = "<!DOCTYPE html><html><head><title>Door Lock Controller</title>";
  page += "<style>body{font-family:Arial, sans-serif;display:flex;flex-direction:column;align-items:center;justify-content:center;height:100vh;margin:0;background-color:#f4f4f9;}h1{font-size:24px;color:#333;}button{font-size:20px;margin:10px;padding:10px 20px;border:none;border-radius:5px;background-color:#007bff;color:white;cursor:pointer;}button:hover{background-color:#0056b3;}</style></head><body>";
  page += "<h1>Door Lock Controller</h1>";
  page += "<button onclick=\"location.href='/lock'\">Lock Door</button>";
  page += "<button onclick=\"location.href='/unlock'\">Unlock Door</button>";
  page += "<button onclick=\"location.href='/settings'\">Settings</button>";
  page += "</body></html>";
  return page;
}

String generateSettingsPage() {
  String page = "<!DOCTYPE html><html><head><title>Settings</title>";
  page += "<style>body{font-family:Arial, sans-serif;display:flex;flex-direction:column;align-items:center;justify-content:center;height:100vh;margin:0;background-color:#f4f4f9;}h1{font-size:24px;color:#333;}input{font-size:18px;margin:10px;padding:5px;width:100px;}button{font-size:20px;margin:10px;padding:10px 20px;border:none;border-radius:5px;background-color:#007bff;color:white;cursor:pointer;}button:hover{background-color:#0056b3;}</style></head><body>";
  page += "<h1>Settings</h1>";
  page += "<form action='/setTimes' method='POST'>";
  page += "Lock Time (ms): <input type='number' name='lockTime' value='" + String(lockTime) + "'><br>";
  page += "Unlock Time (ms): <input type='number' name='unlockTime' value='" + String(unlockTime) + "'><br>";
  page += "Lock Reverse Time (ms): <input type='number' name='lockReverseTime' value='" + String(lockReverseTime) + "'><br>";
  page += "Unlock Reverse Time (ms): <input type='number' name='unlockReverseTime' value='" + String(unlockReverseTime) + "'><br>";
  page += "<button type='submit'>Save</button>";
  page += "</form>";
  page += "<button onclick=\"location.href='/'\">Back</button>";
  page += "</body></html>";
  return page;
}

void loop() {
  server.handleClient();
}

void lockDoor() {
  Serial.println("Locking Door...");

  // Step 1: Power off the motor before changing direction
  digitalWrite(relay3Pin, HIGH);

  // Step 2: Set direction for locking (LOW LOW)
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);

  // Step 3: Power on the motor
  digitalWrite(relay3Pin, LOW);

  // Move in the lock direction for the specified time
  delay(lockTime);

  // Step 4: Power off the motor again before reversing direction
  digitalWrite(relay3Pin, HIGH);

  // Step 5: Reverse direction (HIGH HIGH)
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);

  // Step 6: Power on the motor for reversing
  digitalWrite(relay3Pin, LOW);

  // Move in the reverse direction for the specified time
  delay(lockReverseTime);

  // Step 7: Power off the motor after completing the movement
  stopDoor();
  Serial.println("Door Locked.");
}

void unlockDoor() {
  Serial.println("Unlocking Door...");

  // Step 1: Power off the motor before changing direction
  digitalWrite(relay3Pin, HIGH);

  // Step 2: Set direction for unlocking (HIGH HIGH)
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);

  // Step 3: Power on the motor
  digitalWrite(relay3Pin, LOW);

  // Move in the unlock direction for the specified time
  delay(unlockTime);

  // Step 4: Power off the motor again before reversing direction
  digitalWrite(relay3Pin, HIGH);

  // Step 5: Reverse direction (LOW LOW)
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);

  // Step 6: Power on the motor for reversing
  digitalWrite(relay3Pin, LOW);

  // Move in the reverse direction for the specified time
  delay(unlockReverseTime);

  // Step 7: Power off the motor after completing the movement
  stopDoor();
  Serial.println("Door Unlocked.");
}

void stopDoor() {
  digitalWrite(relay3Pin, HIGH); // Cut power to stop
}
