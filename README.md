# ESP-01 Based Motion Sensor

This project is a very simple motion sensor using an ESP-01 ESP8266 module and
HC-SR501/DSN-FIR800 motion sensor.

Countless other designs using the same ESP module and motion sensor exist, this
project was created as a learning exercise. And because I wanted to.

## PCB

The PCB folder contains a Kicad project for a simple PCB that both the ESP
moodule and motion sensor is mounted to. The board provides for a 3.3V
regulator and USB connector for power input.

## Software

The Software folder contains the Arduino IDE program that I am running on my
modules. It automatically publishes state changes into an MQTT topic based on
the Chip ID number on the microcontroller and also supports OTA updates for the
first 2 minutes after powering on or being reset. The Sketch uses the TX LED as
an activity indicator.

## TODO

* Create a 3D printed case to house the PCB
