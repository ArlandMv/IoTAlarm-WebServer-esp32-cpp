#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const int LED_ON_DURATION = 3000; //30*1000
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif
const int relay = 26;
static bool relayStatus = HIGH; //init depends on module
unsigned long triggerStartTime = 0;
bool alarmActive = false;

const char *SSID = "ingenieria";
const char *PASSWORD = "iping2024";

const char* AUTH_USER = "admin";
const char* AUTH_PASS = "secret";
WebServer server(80);

// DECLARATIONS
void switchRelay();
void connectToWiFi();
void handleTrigger();
void setupOTA();

void setup()
{
  Serial.begin(115200);
  // pinMode(buzzerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, relayStatus); 

  connectToWiFi();
  //void setupOTA();

  server.on("/trigger", HTTP_GET, handleTrigger);

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
  server.handleClient();
  //ArduinoOTA.handle();
}

// IMPLEMENTATIONS
void handleTrigger() {
  /*
  if (!server.authenticate(AUTH_USER, AUTH_PASS)) {
    return server.requestAuthentication();
  }*/
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED turned on");
  switchRelay();
  digitalWrite(relay, relayStatus); 
  
  server.send(200, "text/plain", "LED triggered");
  delay(LED_ON_DURATION);
  
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED turned off");
  switchRelay();
  digitalWrite(relay, relayStatus); 
}


void switchRelay(){
  relayStatus = !relayStatus;
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(SSID, PASSWORD);
  
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Restarting...");
    ESP.restart();
  }
}

// OTA
/*
void setupOTA() {
  ArduinoOTA.setHostname("esp32-doorbell");
  ArduinoOTA.setPassword("admin");
  
  ArduinoOTA.onStart([]() {
    Serial.println("Starting OTA update");
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA update finished");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
}*/