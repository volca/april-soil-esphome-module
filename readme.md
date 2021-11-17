### April soil moisture sensor module

Custom april soil moisture sensor module for esphome

This module allow to connect april soil moisture sensor with esphome.

### How to add the module?

1. Create an empty project with esphome named `april_soil`
2. Copy `april-soil.yaml` and `april_soil_sensor.h` to your project
3. Download docker esphome beta version which supports `ESP32 S2`
```
docker pull esphome/esphome:beta
```
4. See section Secrets to add `secrets.yaml`
5. Compile
```
docker run --rm -v "${PWD}":/config --device=/dev/ttyUSB0 -it esphome/esphome:beta run april-soil.yaml
```

#### Secrets

`april-soil.yaml` contains some directives such as `!secret mqtt_password`, which instructs it to read secrets from a `secrets.yaml` file.

This file is usually not commited to version control, so you'll need to populate your own. Here's an example:

```
# Example secrets.yaml
wifi_ssid: WIFI_NETWORK_NAME
wifi_password: WIFI_PASSWORD
mqtt_broker: MQTT_BROKER
mqtt_password: MQTT_PASSWORD
ota_password: OTA_PASSWORD
```
