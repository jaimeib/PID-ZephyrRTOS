// Zephyr API
#include <net/socket.h>
#include <net/mqtt.h>

//Standard C + POSIX API
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <random/rand32.h>
#include <posix/pthread.h>

//Modules
#include "mqtt_publisher.h"
#include "main.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(net_mqtt_publisher_sample, LOG_LEVEL_DBG);

/* Buffers for MQTT client. */
static uint8_t rx_buffer[APP_MQTT_BUFFER_SIZE];
static uint8_t tx_buffer[APP_MQTT_BUFFER_SIZE];

/* The mqtt client struct */
static struct mqtt_client client_ctx;

/* MQTT Broker details. */
static struct sockaddr_storage broker;

static struct zsock_pollfd fds[1];
static int nfds;

static bool connected;

static void prepare_fds(struct mqtt_client *client)
{
	if (client->transport.type == MQTT_TRANSPORT_NON_SECURE) {
		fds[0].fd = client->transport.tcp.sock;
	}

	fds[0].events = ZSOCK_POLLIN;
	nfds = 1;
}

static void clear_fds(void)
{
	nfds = 0;
}

static int wait(int timeout)
{
	int ret = 0;

	if (nfds > 0) {
		ret = zsock_poll(fds, nfds, timeout);
		if (ret < 0) {
			LOG_ERR("poll error: %d", errno);
		}
	}

	return ret;
}

void mqtt_evt_handler(struct mqtt_client *const client, const struct mqtt_evt *evt)
{
	int err;

	switch (evt->type) {
	case MQTT_EVT_CONNACK:
		if (evt->result != 0) {
			LOG_ERR("MQTT connect failed %d", evt->result);
			break;
		}

		connected = true;
		printf("MQTT client connected!");

		break;

	case MQTT_EVT_DISCONNECT:
		printf("MQTT client disconnected %d", evt->result);

		connected = false;
		clear_fds();

		break;

	case MQTT_EVT_PUBACK:
		if (evt->result != 0) {
			LOG_ERR("MQTT PUBACK error %d", evt->result);
			break;
		}

		printf("PUBACK packet id: %u", evt->param.puback.message_id);

		break;

	case MQTT_EVT_PUBREC:
		if (evt->result != 0) {
			LOG_ERR("MQTT PUBREC error %d", evt->result);
			break;
		}

		printf("PUBREC packet id: %u", evt->param.pubrec.message_id);

		const struct mqtt_pubrel_param rel_param = { .message_id =
								     evt->param.pubrec.message_id };

		err = mqtt_publish_qos2_release(client, &rel_param);
		if (err != 0) {
			LOG_ERR("Failed to send MQTT PUBREL: %d", err);
		}

		break;

	case MQTT_EVT_PUBCOMP:
		if (evt->result != 0) {
			LOG_ERR("MQTT PUBCOMP error %d", evt->result);
			break;
		}

		printf("PUBCOMP packet id: %u", evt->param.pubcomp.message_id);

		break;

	case MQTT_EVT_PINGRESP:
		printf("PINGRESP packet");
		break;

	default:
		break;
	}
}

static char *get_mqtt_topic(void)
{
	return MQTT_TOPIC;
}

static char *get_mqtt_payload(thread_result_t *ptr_result, enum mqtt_qos qos)
{
	static char payload[30];
	thread_result_t data;

	data.type = ptr_result->type;
	data.value = ptr_result->value;

	switch (data.type) {
	case LIGHT:
		snprintk(payload, sizeof(payload), "{light:%f}", data.value);
		break;
	case INTERNAL_TEMPERATURE:
		snprintk(payload, sizeof(payload), "{temperature:%f}", data.value);
		break;
	default:
		break;
	}

	printf("MQTT Payload: %s\n", payload);
	return payload;
}

static int publish(struct mqtt_client *client, thread_result_t *ptr_result, enum mqtt_qos qos)
{
	struct mqtt_publish_param param;

	param.message.topic.qos = qos;
	param.message.topic.topic.utf8 = (uint8_t *)get_mqtt_topic();
	param.message.topic.topic.size = strlen(param.message.topic.topic.utf8);
	param.message.payload.data = get_mqtt_payload(ptr_result, qos);
	param.message.payload.len = strlen(param.message.payload.data);
	param.message_id = sys_rand32_get();
	param.dup_flag = 0U;
	param.retain_flag = 0U;

	return mqtt_publish(client, &param);
}

#define RC_STR(rc) ((rc) == 0 ? "OK" : "ERROR")

#define PRINT_RESULT(func, rc) printf("%s: %d <%s>\n", (func), rc, RC_STR(rc))

static void broker_init(void)
{
	struct sockaddr_in *broker4 = (struct sockaddr_in *)&broker;

	broker4->sin_family = AF_INET;
	broker4->sin_port = htons(MQTT_SERVER_PORT);

	zsock_inet_pton(AF_INET, MQTT_SERVER_ADDRESS, &broker4->sin_addr);
}

static void client_init(struct mqtt_client *client)
{
	mqtt_client_init(client);

	broker_init();

	/* MQTT client configuration */
	client->broker = &broker;
	client->evt_cb = mqtt_evt_handler;
	client->client_id.utf8 = (uint8_t *)MQTT_CLIENTID;
	client->client_id.size = strlen(MQTT_CLIENTID);
	client->password = NULL;
	client->user_name = NULL;
	client->protocol_version = MQTT_VERSION_3_1_1;

	/* MQTT buffers configuration */
	client->rx_buf = rx_buffer;
	client->rx_buf_size = sizeof(rx_buffer);
	client->tx_buf = tx_buffer;
	client->tx_buf_size = sizeof(tx_buffer);
	client->transport.type = MQTT_TRANSPORT_NON_SECURE;
}

/* In this routine we block until the connected variable is 1 */
static int try_to_connect(struct mqtt_client *client)
{
	int rc, i = 0;

	while ((i++ < APP_CONNECT_TRIES) && !connected) {
		client_init(client);

		rc = mqtt_connect(client);
		if (rc != 0) {
			PRINT_RESULT("mqtt_connect", rc);
			k_sleep(K_MSEC(APP_SLEEP_MSECS));
			continue;
		}

		prepare_fds(client);

		wait(APP_CONNECT_TIMEOUT_MS);

		mqtt_input(client);

		if (!connected) {
			mqtt_abort(client);
		}
	}

	if (connected) {
		return 0;
	}

	return -EINVAL;
}

/*
static int process_mqtt_and_sleep(struct mqtt_client *client, int timeout)
{
	int64_t remaining = timeout;
	int64_t start_time = k_uptime_get();
	int rc;

	while (remaining > 0 && connected) {
		if (wait(remaining)) {
			rc = mqtt_input(client);
			if (rc != 0) {
				PRINT_RESULT("mqtt_input", rc);
				return rc;
			}
		}

		rc = mqtt_live(client);
		if (rc != 0 && rc != -EAGAIN) {
			PRINT_RESULT("mqtt_live", rc);
			return rc;
		} else if (rc == 0) {
			rc = mqtt_input(client);
			if (rc != 0) {
				PRINT_RESULT("mqtt_input", rc);
				return rc;
			}
		}

		remaining = timeout + start_time - k_uptime_get();
	}

	return 0;
}
*/

#define SUCCESS_OR_EXIT(rc)                                                                        \
	{                                                                                          \
		if (rc != 0) {                                                                     \
			return 1;                                                                  \
		}                                                                                  \
	}
#define SUCCESS_OR_BREAK(rc)                                                                       \
	{                                                                                          \
		if (rc != 0) {                                                                     \
			break;                                                                     \
		}                                                                                  \
	}

int mqtt_publisher(void *ptr_result)
{
	//Sincronization variables for the result:
	extern pthread_mutex_t mutex_result;
	extern pthread_cond_t cond_result;
	extern bool new_result_for_publisher;

	//Sincronization variables for the publisher is ready:
	extern pthread_mutex_t mutex_publisher_ready;
	extern pthread_cond_t cond_publisher_ready;
	extern bool publisher_ready;

	thread_result_t last_result;

	printf("MQTT publisher thread started\n");
	int r = 0;

	//Try to connect to the client:
	printf("attempting to connect: ");
	r = try_to_connect(&client_ctx);
	PRINT_RESULT("try_to_connect", r);
	SUCCESS_OR_EXIT(r);

	//Signal the main thread that we are connected (ready)
	pthread_mutex_lock(&mutex_publisher_ready);
	publisher_ready = true;
	pthread_cond_signal(&cond_publisher_ready);
	pthread_mutex_unlock(&mutex_publisher_ready);
	printf("MQTT publisher thread is ready\n");

	while (connected) {
		// Wait for a new result
		pthread_mutex_lock(&mutex_result);

		// Wait for the signal from the sensor thread
		while (!new_result_for_publisher) {
			pthread_cond_wait(&cond_result, &mutex_result);
		}

		// Copy the result
		last_result.type = ((thread_result_t *)ptr_result)->type;
		last_result.value = ((thread_result_t *)ptr_result)->value;
		new_result_for_publisher = false;

		// Unlock the mutex
		pthread_mutex_unlock(&mutex_result);

		//Publish the message:
		r = publish(&client_ctx, &last_result, MQTT_QOS_0_AT_MOST_ONCE);
		PRINT_RESULT("mqtt_publish", r);
		SUCCESS_OR_BREAK(r);
	}

	r = mqtt_disconnect(&client_ctx);
	PRINT_RESULT("mqtt_disconnect", r);

	return r;
}
