# Guía de inicio

## 1. Instalación del sistema operativo

Zephyr se ha instalado correctamente en Ubuntu 20.04 LTS siguiendo las indicaciones de la documentación oficial, para la versión sobre el entorno virtual en Python, que es la recomendada en la propia documentación.

> [Documentación oficial del sistema operativo Zephyr Project de Linux Fundation](https://docs.zephyrproject.org/latest/index.html) -
> [Repositorio oficial](https://github.com/zephyrproject-rtos/zephyr)

## 2. Prueba básica con la placa STM32F769 Discovery Kit

Para empezar a trabajar es necesario activar el entorno virtual de Python:

```bash
source ~/zephyrproject/.venv/bin/activate
```

La compilación de un programa se realiza con `west build -b <board-name> example`.
Para pasar el binario a la memoria de la placa se ejecuta el comando `west flash`.
Es necesario que la compilación se realice dentro del directorio `zephyr/`.

> En algunos ejemplos se utiliza impresión por el puerto serie de la placa (dispositivo _/dev/ttyACM0_ al conectar por USB). Para poder visualizar la información, se puede utilizar un terminal gráfico como CuteCom. Se puede instalar facilmente en Ubuntu con `apt-get install cutecom`.

### Blinky Sample

La muestra hace parpadear un LED usando la API _GPIO_.

```
west build -p auto -b stm32f769i_disco samples/basic/blinky
```

### Hello World! Sample

Imprime "Hello World" en la consola, conectada al puerto serie de la placa (_dev/ttyACM0_).

```
west build -p auto -b stm32f769i_disco samples/hello_world
```

### Button Sample

Imprime un mensaje en la consola cada vez que se presiona un botón, haciendo uso de entrada _GPIO_ con interrupciones.

```
west build -p auto -b stm32f769i_disco samples/basic/button
```

### Basic Thread Sample

Este ejemplo demuestra la generación de varios threads mediante `K_THREAD_DEFINE()`. Genera tres hilos. Luego, cada thread se define en tiempo de compilación utilizando `K_THREAD_DEFINE`.

Los dos primeros controlan cada uno un LED. Estos LED, led0 y led1, se ejecuta en un bucle y la temporización de los leds es controlada por funciones separadas:

-   `blink0()` controla led0 y tiene un ciclo de suspensión de 100ms
-   `blink1()` controla led1 y tiene un ciclo de suspensión de 1000 ms

Cuando cualquiera de estos thread alterna su LED, también envía información a una cola FIFO que identifica el thread/LED y cuántas veces se ha activado.

El tercer hilo usa `printk()` para imprimir la información agregada a la cola FIFO en la consola del dispositivo.

```
west build -p auto -b stm32f769i_disco samples/basic/threads
```

### Touchscreen Sample

```
west build -p auto -b stm32f769i_disco samples/drivers/kscan_touch/
```

## 3. Prueba básica de los sensores

## 4. Prueba básica de las funciones de red

Los ejemplos de red se pueden utilizar conectando un cable normal Ethernet entre la placa y el host. Para configurar los parámetros del proyecto, en este caso de red, se hace uso de un archivo de configuración `prj.conf`, presente en el directorio principal del proyecto.

La configuración de direcciones IP de área local (LAN) por defecto es la siguiente:

```
CONFIG_NET_CONFIG_MY_IPV4_ADDR="192.0.2.1"
CONFIG_NET_CONFIG_PEER_IPV4_ADDR="192.0.2.2"
```

### Socket Echo Client Sample

La aplicación implementa un cliente UDP/TCP que enviará paquetes IPv4 o IPv6, esperará a que se envíen los datos y luego verificará que coincidan con los datos que se enviaron.

```
west build .p auto -b stm32f769i_disco samples/net/sockets/echo_client -- -DCONF_FILE="prj.conf overlay-log.conf"
```

### Socket Echo Server Sample

La aplicación implementa un servidor UDP/TCP que complementa la aplicación de muestra del cliente de eco: el servidor de eco escucha los paquetes IPv4 o IPv6 entrantes (enviados por el cliente de eco) y simplemente los devuelve.

```
west build -p auto -b stm32f769i_disco samples/net/sockets/echo_server/ -- -DCONF_FILE="prj.conf overlay-log.conf"
```

> Para enviar tráfico al servidor (la placa), podemos utilizar un generador de paquetes tipo Package Sender (`apt-get install packetsender`), o a traves de la shell del tipo bash de Linux:

```bash
echo "This is my TCP message" > /dev/tcp/192.0.2.1/4242
echo "This is my UDP message" > /dev/upd/192.0.2.1/4242
```

### MQTT Publisher Sample

[MQTT (MQ Telemetry Transport)](https://mqtt.org/) es un protocolo ligero de mensajería de publicación/suscripción optimizado para sensores pequeños y dispositivos móviles.
La aplicación de prueba es un cliente MQTT v3.1.1 que envía mensajes MQTT PUBLISH a un broker MQTT.

```
west build -p auto -b stm32f769i_disco samples/net/mqtt_publisher/
```

En una terminal de Linux, ejecutaremos el broker de MQTT _mosquitto_ (se puede instalar con `apt-get mosquitto`) y lo ejecutaremos con su configuración por defecto, es decir utilando el puerto 1883:

```
mosquitto -v
```

> En el caso de que muestre un mensaje de error, porque el puerto ya esta en uso, es necesario matar al demonio de _mosquitto_ ya ejecutandose en el sistema, para liberar el puerto por defecto. (`kill -9 <PID>`)

## 5. Prueba básica con la plataforma ThingsBoard

> [Documentación oficial de ThingsBoard Community Edition](https://thingsboard.io/docs/) -
> [Repositorio oficial](https://github.com/thingsboard/thingsboard)
