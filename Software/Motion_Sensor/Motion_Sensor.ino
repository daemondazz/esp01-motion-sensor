//#define DEBUG
#include "DebugUtils.h"

#define SOFTWARE_VERSION "0.1.0"

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

bool        ota_upgrades = true;                  // Set this to false to disable OTA updates
const char* ota_password = "<update password>";   // Password to use to secure OTA updates if enabled
const char* mqtt_server  = "<mqtt broker>";       // IP Address or Hostname of MQTT server
const int   mqtt_port    = <mqtt broker port>;    // Port of MQTT server, default is 1833
const char* wifi_ssid    = "<wireless ssid>";     // SSID of the wireless network to connect to
const char* wifi_pass    = "<wireless password>"; // Password for wireless network

const int HARDWARE_ID = ESP.getChipId();

int flash_mode_pin = 0;
int led_pin = 1;
int sensor_pin = 2;
volatile byte current_state = LOW;
byte last_state = LOW;

unsigned long keepalive_time = 10000;
unsigned long last_update = 0;
char mqtt_clientid[35];
char mqtt_name_topic[100];
char mqtt_name_payload[100];
char mqtt_state_topic[100];


WiFiClient espClient;
PubSubClient mqtt(mqtt_server, mqtt_port, espClient);


void handle_sensor_change() {
  current_state = digitalRead(sensor_pin);
}


void mqtt_connect() {
  DEBUG_PRINTF("Attempting connection to MQTT server at %s as %s: ", mqtt_server, mqtt_clientid)

  while (! mqtt.connected()) {
    if (mqtt.connect(mqtt_clientid)) {
      DEBUG_PRINT(" Connected!\n");
    } else {
      DEBUG_PRINT(".")
      delay(5000);
    }
  }
}


void setup() {
  SERIAL_BEGIN(115200)
  DEBUG_PRINTF("\n * * * VK3GO Motion Sensor booting on Board ID %X * * *\n", HARDWARE_ID)

  // Build our strings that include the HARDWARE_ID
  sprintf(mqtt_clientid, "VK3GO_MS01_%X", HARDWARE_ID);
  sprintf(mqtt_name_topic, "sensors/binary/ms01_%x/name", HARDWARE_ID);
  sprintf(mqtt_name_payload, "VK3GO Motion Sensor ID %X Version %s (OTA Enabled)", HARDWARE_ID, SOFTWARE_VERSION);
  sprintf(mqtt_state_topic, "sensors/binary/ms01_%x/state", HARDWARE_ID);

  // Connect to WiFI Network
  DEBUG_PRINT("Starting Connection to WiFi Network: ")
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINT(".")
    delay(500);
  }
  DEBUG_PRINTF(" Connected!\n -> IP Address: %s\n", WiFi.localIP().toString().c_str())

  // Set up ArduinoOTA for over the air updates, reuse MQTT client ID as hostname
  ArduinoOTA.setHostname(mqtt_clientid);
  ArduinoOTA.setPassword(ota_password);
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { digitalWrite(led_pin, !digitalRead(led_pin)); });
  ArduinoOTA.begin();

  // Set up the GPIO Pins
  DEBUG_PRINT("Setting up GPIO0 as GND for sensor\n")
  pinMode(flash_mode_pin, OUTPUT);
  digitalWrite(flash_mode_pin, LOW);
  DEBUG_PRINT("Setting up GPIO2 as sensor input\n")
  pinMode(sensor_pin, INPUT_PULLUP);
  current_state = digitalRead(sensor_pin);
  attachInterrupt(digitalPinToInterrupt(sensor_pin), handle_sensor_change, CHANGE);
  DEBUG_PRINT("Setting up GPIO1 as activity indicator");
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, !current_state);
}


void loop() {
  // Make sure we're connected to the MQTT server
  if (! mqtt.connected()) {
    mqtt_connect();
  }

  // Handle any incoming MQTT messages / keepalives
  mqtt.loop();

  // Handle any OTA requests, but only for the first 2 minutes after a reboot
  if (ota_upgrades) {
    ArduinoOTA.handle();
    if (millis() > 120000) {
      ota_upgrades = false;
      // Remove OTA Enabled message from name payload
      sprintf(mqtt_name_payload, "VK3GO Motion Sensor ID %X Version %s", HARDWARE_ID, SOFTWARE_VERSION);
    }
  }

  // Publish our version message if it's been 5 minutes since the last time
  if (last_update == 0 || (millis() - last_update) >= keepalive_time) {
    mqtt.publish(mqtt_name_topic, mqtt_name_payload);
    last_update = millis();
  }

  // Check for any sensor changes
  if (current_state != last_state) {
    DEBUG_PRINTF("Sensor has changed state: %d\n", current_state)

    // Set the LED
    digitalWrite(led_pin, !current_state);

    // If it's HIGH, publish an 'on' message
    if (current_state == HIGH) {
      mqtt.publish(mqtt_state_topic, "on", true);

      // Otherwise publish an 'off' message
    } else {
      mqtt.publish(mqtt_state_topic, "off", true);
    }

    // Update our last state so we know we've handled it
    last_state = current_state;
  }
}

