#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
//#include <ESP_Mail_Client.h>
//#include "MailClient.h"
#include <time.h>

const int LED_ON_DURATION = 3000; // 30*1000
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif
const int relay = 26;
static bool relayStatus = HIGH; // init depends on module
unsigned long triggerStartTime = 0;
bool alarmActive = false;

const char *SSID = "ingenieria";
const char *PASSWORD = "iping2024";

const char *AUTH_USER = "admin";
const char *AUTH_PASS = "secret";

const char *NTP_SERVER = "cl.pool.ntp.org";
const long  GMT_OFFSET = -4*3600; //  (UTC -3 hours) or (UTC -4 hours)
// depending on location: https://www.timeanddate.com/time/zone/chile
const int   DAYLIGHT_OFFSET = 0; 

WebServer server(80);

//SMTPSession smtp;
//SMTP_Message message;

// DECLARATIONS
void switchRelay();
void connectToWiFi();
void handleTrigger();
void setupTime();
void printLocalTime();
//void setupMailClient();
//void sendMail(String timestamp); // send mail after a get request
//void setupOTA();

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay, OUTPUT); //buzzerPin
  digitalWrite(relay, relayStatus); //init off state

  connectToWiFi();
  setupTime();
  printLocalTime();

  // void setupMailClient();
  // void setupOTA();

  server.on("/trigger", HTTP_GET, handleTrigger);

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connectToWiFi();
  }
  server.handleClient();
  //timeClient.update();

  // ArduinoOTA.handle();
}

// IMPLEMENTATIONS

void switchRelay()
{
  relayStatus = !relayStatus;
}

void connectToWiFi()
{
  Serial.println("Connecting to WiFi...");
  WiFi.begin(SSID, PASSWORD);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20)
  {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nFailed to connect to WiFi. Restarting...");
    ESP.restart();
  }
}

void handleTrigger()
{
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

void setupTime() { 
  configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);
  Serial.println("NTP time initialized");
}
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.println(&timeinfo, "Current time: %Y-%m-%d %H:%M:%S");
}


/*
void setupMailClient()
{
  smtp.debug(1);
  smtp.callback(smtpCallback);

  ESP_Mail_Session session;
  session.server.host_name = "smtp.yourserver.com"; // Replace with your SMTP server
  session.server.port = 587;                        // Typically 465 or 587
  session.login.email = "your_email@example.com";   // Replace with your email
  session.login.password = "your_password";         // Replace with your email password
}*/

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