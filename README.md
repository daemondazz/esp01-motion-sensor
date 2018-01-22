# ESP-01 Based Motion Sensor

This project is a very simple motion sensor using an ESP-01 ESP8266 module and
HC-SR501/DSN-FIR800 motion sensor.

Countless other designs using the same ESP module and motion sensor exist, this
project was created as a learning exercise. And because I wanted to.

## PCB

The PCB folder contains a Kicad project for a simple PCB that both the ESP
module and motion sensor are mounted to. It has the following features:

* Allows direct mounting of the ESP-01 module
* Allows direct mounting of the HC-SR501 sensor as a daughter board
* Uses a micro-USB connector for power input
* Contains a 3.3V regulator for powering the ESP-01
* Includes required pull-up resistors to allow the ESP-01 to boot reliably and
  then use the GPIO2 pin for the sensor input

## Software

The Software folder contains the Arduino sketch that I am running on my modules. 
It has the following features:

* Automatically publishes state changes into an MQTT topic based on the Chip ID 
  number embedded in the ESP-01
* Automatically publishes a name/version/OTA state message into an MQTT topic
  based on the Chip ID embedded into the ESP-01 every 10s as a keepalive
* Supports password authenticated OTA updates for the first 2 minutes after 
  powering on or being reset
* Uses the TX LED as an activity indicator

## TODO

* Create a 3D printed case to house the PCB
* Allow creation of a command MQTT topic to issue commands
