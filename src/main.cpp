#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <time.h>
#include <ESP_Mail_Client.h>
/*Note: library update from v2.x.x to v3.x.x.
    "ESP_Mail_Session" changes to "Session_Config"
    ESP_Mail_Session session => Session_Config config
*/
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "arlandmichelenav@gmail.com"
#define AUTHOR_PASSWORD "gawh rxmo jkor fxhf"
#define RECIPIENT_EMAIL "arlandmvillegas@gmail.com"

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
const long GMT_OFFSET = -4 * 3600; //  (UTC -3 hours) or (UTC -4 hours)
// depending on location: https://www.timeanddate.com/time/zone/chile
const int DAYLIGHT_OFFSET = 0;

WebServer server(80);

SMTPSession smtp;
Session_Config config;
void smtpCallback(SMTP_Status status);

// DECLARATIONS
void switchRelay();
void connectToWiFi();
void handleTrigger();
void setupTime();
void printLocalTime();
void setupMailClient();
bool sendNotification(const char *subject, const char *message);
// void setupOTA();

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay, OUTPUT);           // buzzerPin
  digitalWrite(relay, relayStatus); // init off state

  connectToWiFi();
  setupTime();
  printLocalTime();
  setupMailClient();
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
  // timeClient.update();
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

void setupTime()
{
  configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);
  Serial.println("NTP time initialized");
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.println(&timeinfo, "Current time: %Y-%m-%d %H:%M:%S");
}

void setupMailClient()
{
  smtp.debug(1);
  smtp.callback(smtpCallback);

  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;

  if (!smtp.connect(&config))
  {
    MailClient.printf("Connection error, Status Code: %d, Error Code: %d, Reason: %s\n", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }
  else
  {
    Serial.println("Connected to SMTP server");
  }
  if (!smtp.isLoggedIn())
  {
    Serial.println("Error, Not yet logged in.");
  }
  else
  {
    if (smtp.isAuthenticated())
      Serial.println("Successfully logged in.");
    else
      Serial.println("Connected with no Auth.");
  }
}

bool sendNotification(const char *subject, const char *message)
{
  if (!smtp.isAuthenticated())
  {
    Serial.println("SMTP not authenticated. Reconnecting...");
    if (!smtp.connect(&config))
    {
      Serial.println("Failed to reconnect to SMTP server");
      return false;
    }
  }

  SMTP_Message email;
  email.sender.name = "ESP32";
  email.sender.email = AUTHOR_EMAIL;
  email.subject = subject;
  email.addRecipient("Recipient", RECIPIENT_EMAIL);
  email.text.content = message;

  if (!MailClient.sendMail(&smtp, &email))
  {
    Serial.println("Error Reason:");
    Serial.println(smtp.errorReason());
    MailClient.printf("Error, Status Code: %d, Error Code: %d, Reason: %s\n", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    MailClient.printf("Free Heap: %d\n", MailClient.getFreeHeap());
    return false;
  }
  else
  {
    Serial.println("Notification sent successfully");
    return true;
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

  char timestamp[30];
  struct tm timeinfo;
  if (getLocalTime(&timeinfo))
  {
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);
  }
  else
  {
    strcpy(timestamp, "Time not available");
  }

  char subject[50];
  snprintf(subject, sizeof(subject), "Alarma IoT %s", timestamp);

  char message[100];
  snprintf(message, sizeof(message), "ALARMA ACTIVADA  %s", timestamp);

  if (sendNotification(subject, message))
  {
    server.send(200, "text/plain", "ALARM triggered and notification sent");
  }
  else
  {
    server.send(500, "text/plain", "ALARM triggered but notification failed");
  }
  server.send(200, "text/plain", "LED triggered");
  delay(LED_ON_DURATION);

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED turned off");
  switchRelay();
  digitalWrite(relay, relayStatus);
}

void smtpCallback(SMTP_Status status)
{
  Serial.println("----SMTP_Status----");
  Serial.println(status.info());
  if (status.success())
  {
    Serial.println("----------------");
    MailClient.printf("Message sent success: %d\n", status.completedCount());
    MailClient.printf("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");
    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
        {
            SMTP_Result result = smtp.sendingResult.getItem(i);

            MailClient.printf("Message No: %d\n", i + 1);
            MailClient.printf("Status: %s\n", result.completed ? "success" : "failed");
            MailClient.printf("Recipient: %s\n", result.recipients.c_str());
            MailClient.printf("Subject: %s\n", result.subject.c_str());
        }
        Serial.println("----------------\n");
        smtp.sendingResult.clear();
  }
  else
  {
    Serial.println("SMTP_Status Error");
    Serial.println("sendingResult");
    Serial.println(smtp.sendingResult.size());
  }
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