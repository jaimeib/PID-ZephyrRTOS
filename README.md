# Práctica Zephyr-RTOS con STM32F769-Discovery-Kit

**Práctica de colaboración en el Dpto. Ingeniería Informática y Electrónica de la Universidad de Cantabria**

> _Implantación de plataforma docente para sistemas empotrados y tecnologías industria 4.0_

### Zephyr Project

[Zephyr Project](https://zephyrproject.org/) es un pequeño sistema operativo de tiempo real (RTOS) para dispositivos con recursos limitados y embebidos (IoT) que admite múltiples arquitecturas. Se publica bajo la Licencia Apache 2.0. Zephyr incluye un kernel y todos los componentes y bibliotecas, controladores de dispositivos, pilas de protocolos, sistemas de archivos y actualizaciones de firmware necesarios para desarrollar una aplicación software completa.

<img src="https://01.org/sites/default/files/zephyr-new-release-v1.8-2017.png">

### Placa STM32F769 Discovery Kit

[STM32F7 discovery kit](https://www.st.com/en/evaluation-tools/32f769idiscovery.html) permite a los usuarios desarrollar y compartir aplicaciones con los microcontroladores de la serie STM32F7 basados en el núcleo [ARM®Cortex®-M7](https://www.arm.com/products/silicon-ip-cpu/cortex-m/cortex-m7).

Permite una amplia diversidad de aplicaciones que se benefician de las características de audio, soporte multisensor, gráficos, seguridad, video y conectividad de alta velocidad. El soporte de conectividad Arduino™ proporciona capacidades de expansión ilimitadas con una gran variedad de placas complementarias especializadas, como la [Grove Base Shield V2.0 for Arduino](https://www.seeedstudio.com/Base-Shield-V2.html).

<img src="https://res.cloudinary.com/rsc/image/upload/b_rgb:FFFFFF,c_pad,dpr_1.0,f_auto,q_auto,w_700/c_pad,w_700/F1231055-01" width=49%><img src="https://www.kiwi-electronics.nl/image/cache/catalog/product/k8cxcch1/SS-103030000_1-1280x853h.jpg" width=50%>

### ThingsBoard IoT Platform

[ThingsBoard](https://thingsboard.io/) es una plataforma de IoT (Internet of things o Internet de las cosas) de código abierto. Está centrada en permitir un rápido desarrollo, gestión y escalado de proyectos relacionados con esta tecnología. Además, en ella tendremos acceso a una solución local o en la nube, que estará lista para usar y que habilitará la infraestructura del lado del servidor para las aplicaciones que vayan a utilizarse.

Es compatible con los protocolos de IoT estándar de la industria: [MQTT](https://mqtt.org/), CoAP y HTTP. Consigue combinar escalabilidad, tolerancia a los fallos y un buen rendimiento a la hora capturar los datos del dispositivo para su procesamiento y control. Esto es posible gracias a que dispone de un servidor de puerta de enlace que se encarga de la comunicación con los dispositivos conectados a la red. Así se logra una gestión ágil y en permanente actualización.

<img src="https://thingsboard.io/images/install/thingsboard-ce-smart-energy.png">

## Guía de inicio

### 1. Instalación del sistema operativo

Zephyr se ha instalado correctamente en Ubuntu 20.04 LTS siguiendo las indicaciones de la documentación oficial, para la versión sobre el entorno virtual en Python, que es la recomendada en la propia documentación.

> [Documentación oficial del sistema operativo Zephyr Project de Linux Fundation](https://docs.zephyrproject.org/latest/index.html) -
[Repositorio oficial](https://github.com/zephyrproject-rtos/zephyr)

### 2. Prueba básica con la placa STM32F769 Discovery Kit

Para empezar a trabajar es necesario activar el entorno virtual de Python:

```bash
source ~/zephyrproject/.venv/bin/activate
```

La compilación de un programa se realiza con `west build -b <board-name> example`.
Para pasar el binario a la memoria de la placa se ejecuta el comando `west flash`.
Es necesario que la compilación se realice dentro del directorio `zephyr/`.

> En algunos ejemplos se utiliza impresión por el puerto serie de la placa (dispositivo /dev/ttyACM0 al conectar por USB). Para poder visualizar la información, se puede utilizar un terminal gráfico como CuteCom. Se puede instalar facilmente en Ubuntu con `apt-get install cutecom`.

#### Blinky Sample

La muestra hace parpadear un LED usando la API _**GPIO**_.
```
west build -p auto -b stm32f769i_disco samples/basic/blinky
```

#### Hello World! Sample

Imprime "Hello World" en la consola, conectada al puerto serie de la placa (dev/ttyACM0).
```
west build -p auto -b stm32f769i_disco samples/hello_world
```

#### Button Sample

Imprime un mensaje en la consola cada vez que se presiona un botón, haciendo uso de entrada GPIO con interrupciones.
```
west build -p auto -b stm32f769i_disco samples/basic/button
```

#### Basic Thread Sample

Este ejemplo demuestra la generación de varios threads mediante `K_THREAD_DEFINE()`. Genera tres hilos. Luego, cada thread se define en tiempo de compilación utilizando `K_THREAD_DEFINE`.

Los dos primeros controlan cada uno un LED. Estos LED, led0 y led1,  se ejecuta en un bucle y la temporización de los leds es controlada por funciones separadas:
- `blink0()` controla led0 y tiene un ciclo de suspensión de 100ms
- `blink1()` controla led1 y tiene un ciclo de suspensión de 1000 ms

Cuando cualquiera de estos thread alterna su LED, también envía información a una cola FIFO que identifica el thread/LED y cuántas veces se ha activado.

El tercer hilo usa `printk()` para imprimir la información agregada a la cola FIFO en la consola del dispositivo.
```
west build -p auto -b stm32f769i_disco samples/basic/threads
```

#### Touchscreen Sample

```
west build -p auto -b stm32f769i_disco samples/drivers/kscan_touch/
```

### 3. Prueba básica de los sensores

### 4. Prueba básica de las funciones de red

Los ejemplos de red se pueden utilizar conectando un cable normal Ethernet entre la placa y el host. Para configurar los parámetros del proyecto, en este caso de red, se hace uso de un archivo de configuración `prj.conf`, presente en el directorio principal del proyecto. 

La configuración de direcciones IP de área local (LAN) por defecto es la siguiente:

```
CONFIG_NET_CONFIG_MY_IPV4_ADDR="192.0.2.1"
CONFIG_NET_CONFIG_PEER_IPV4_ADDR="192.0.2.2"
```

#### Socket Echo Client Sample 

La aplicación implementa un cliente UDP/TCP que enviará paquetes IPv4 o IPv6, esperará a que se envíen los datos y luego verificará que coincidan con los datos que se enviaron.

```
west build .p auto -b stm32f769i_disco samples/net/sockets/echo_client -- -DCONF_FILE=<config file to use>
``` 

#### Socket Echo Server Sample

La aplicación implementa un servidor UDP/TCP que complementa la aplicación de muestra del cliente de eco: el servidor de eco escucha los paquetes IPv4 o IPv6 entrantes (enviados por el cliente de eco) y simplemente los devuelve.

```
west build -p auto -b stm32f769i_disco samples/net/sockets/echo_server/ -- -DCONF_FILE="prj.conf overlay-log.conf"
```

> Para enviar tráfico al servidor (la placa), podemos utilizar un generador de paquetes tipo Package Sender (`apt-get install packetsender`), o a traves de la  shell del tipo bash de Linux: 

```bash
echo "This is my TCP message" > /dev/tcp/192.0.2.1/4242
echo "This is my UDP message" > /dev/upd/192.0.2.1/4242
```

#### MQTT Publisher Sample

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

### 5. Prueba básica con la plataforma ThingsBoard

> [Documentación oficial de ThingsBoard Community Edition](https://thingsboard.io/docs/) -
[Repositorio oficial](https://github.com/thingsboard/thingsboard)

## Propuesta de práctica
