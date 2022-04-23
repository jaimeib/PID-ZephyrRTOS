/* MQTT PUBLISHER CONFIGURATION*/

//TOPICS:
#define TOPIC_INTERNAL_TEMPERATURE "internal_temperature"
#define TOPIC_LIGHT "light"

//NETWORK:

#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef CONFIG_NET_CONFIG_SETTINGS
#ifdef CONFIG_NET_IPV6
#define ZEPHYR_ADDR CONFIG_NET_CONFIG_MY_IPV6_ADDR
#define SERVER_ADDR CONFIG_NET_CONFIG_PEER_IPV6_ADDR
#else
#define ZEPHYR_ADDR CONFIG_NET_CONFIG_MY_IPV4_ADDR
#define SERVER_ADDR CONFIG_NET_CONFIG_PEER_IPV4_ADDR
#endif
#else
#ifdef CONFIG_NET_IPV6
#define ZEPHYR_ADDR "2001:db8::1"
#define SERVER_ADDR "2001:db8::2"
#else
#define ZEPHYR_ADDR "192.168.1.101"
#define SERVER_ADDR "192.168.1.10"
#endif
#endif

#if defined(CONFIG_SOCKS)
#define SOCKS5_PROXY_ADDR SERVER_ADDR
#define SOCKS5_PROXY_PORT 1080
#endif

#define SERVER_HOSTNAME "demo.thingsboard.io"

#define SERVER_PORT 1883

#define APP_CONNECT_TIMEOUT_MS 2000
#define APP_SLEEP_MSECS 500

#define APP_CONNECT_TRIES 3

#define APP_MQTT_BUFFER_SIZE 128

#define MQTT_CLIENTID "zephyr_publisher"

#endif

// FUNCTIONS:
int mqtt_publisher(void *ptr_result);
