/* MQTT PUBLISHER CONFIGURATION*/

// Sensors values to publish
#define TEMPERATURE_VALUE "internal_temperature"
#define LIGHT_VALUE "light"

//NETWORK:
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MQTT_SERVER_HOST "demo.thingsboard.io"
#define MQTT_SERVER_PORT 1883
#define MQTT_TOPIC "v1/devices/me/telemetry"

#define APP_CONNECT_TIMEOUT_MS 2000
#define APP_SLEEP_MSECS 500

#define APP_CONNECT_TRIES 3

#define APP_MQTT_BUFFER_SIZE 128

#define MQTT_CLIENTID "zephyr_publisher"

#endif

// FUNCTIONS:
int mqtt_publisher(void *ptr_result);
