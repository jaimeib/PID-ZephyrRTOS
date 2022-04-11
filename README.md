# Práctica Zephyr Project OS con STM32F769 Discovery Kit

**Práctica de colaboración en el Dpto. Ingeniería Informática y Electrónica de la Universidad de Cantabria**

_Implantación de plataforma docente para sistemas empotrados y tecnologías industria 4.0_

### Zephyr Project

[Zephyr Project](https://zephyrproject.org/) es un pequeño sistema operativo de tiempo real (RTOS) para dispositivos con recursos limitados y embebidos (IoT) que admite múltiples arquitecturas. Se publica bajo la Licencia Apache 2.0. Zephyr incluye un kernel y todos los componentes y bibliotecas, controladores de dispositivos, pilas de protocolos, sistemas de archivos y actualizaciones de firmware necesarios para desarrollar una aplicación software completa.

<img src="https://01.org/sites/default/files/zephyr-new-release-v1.8-2017.png">

### Placa STM32F769 Discovery Kit

[STM32F7 discovery kit](https://www.st.com/en/evaluation-tools/32f769idiscovery.html) permite a los usuarios desarrollar y compartir aplicaciones con los microcontroladores de la serie STM32F7 basados en el núcleo [ARM®Cortex®-M7](https://www.arm.com/products/silicon-ip-cpu/cortex-m/cortex-m7).

Permite una amplia diversidad de aplicaciones que se benefician de las características de audio, soporte multisensor, gráficos, seguridad, video y conectividad de alta velocidad. El soporte de conectividad Arduino™ proporciona capacidades de expansión ilimitadas con una gran variedad de placas complementarias especializadas, como la [Grove Base Shield V2.0 for Arduino](https://www.seeedstudio.com/Base-Shield-V2.html).

<img src="https://res.cloudinary.com/rsc/image/upload/b_rgb:FFFFFF,c_pad,dpr_1.0,f_auto,q_auto,w_700/c_pad,w_700/F1231055-01" width=49%><img src="https://www.kiwi-electronics.nl/image/cache/catalog/product/k8cxcch1/SS-103030000_1-1280x853h.jpg" width=50%>

### ThingsBoard IoT Platform

[ThingsBoard](https://thingsboard.io/) es una plataforma de IoT (Internet of things o Internet de las cosas) de código abierto. Está centrada en permitir un rápido desarrollo, gestión y escalado de proyectos relacionados con esta tecnología. Además, en ella tendremos acceso a una solución local o en la nube, que estará lista para usar y que habilitará la infraestructura del lado del servidor para las aplicaciones que vayan a utilizarse.

Es compatible con los protocolos de IoT estándar de la industria: MQTT, CoAP y HTTP. Consigue combinar escalabilidad, tolerancia a los fallos y un buen rendimiento a la hora capturar los datos del dispositivo para su procesamiento y control. Esto es posible gracias a que dispone de un servidor de puerta de enlace que se encarga de la comunicación con los dispositivos conectados a la red. Así se logra una gestión ágil y en permanente actualización.

<img src="https://thingsboard.io/images/install/thingsboard-ce-smart-energy.png">

## Guía de inicio

### 1. Instalación del sistema operativo

Zephyr se ha instalado correctamente en Ubuntu 20.04 LTS siguiendo las indicaciones de la documentación oficial, para la versión sobre el entorno virtual en Python, que es la recomendada en la propia documentación.

[Documentación oficial del sistema operativo Zephyr Project de Linux Fundation](https://docs.zephyrproject.org/latest/index.html) -
[Repositorio oficial](https://github.com/zephyrproject-rtos/zephyr)

### 2. Prueba básica con la placa STM32F769 Discovery Kit

Para empezar a trabajar es necesario activar el entorno virtual de Python:

```
source ~/zephyrproject/.venv/bin/activate
```

La compilación de un programa se realiza con `west build -b <board-name> example`.
Para pasar el binario a la memoria de la placa se ejecuta el comando `west flash`.
Es necesario que la compilación se realice dentro del directorio `zephyr/`.

En algunos ejemplos se utiliza impresión por el puerto serie de la placa (dispositivo /dev/ttyACM0 al conectar por USB). Para poder visualizar la información, se puede utilizar un terminal gráfico como CuteCom. Se puede instalar facilmente en Ubuntu con `apt-get install cutecom`.

#### Blinky Sample

```
west build -p auto -b stm32f769i_disco samples/basic/blinky
```

#### Hello World! Sample

```
west build -p auto -b stm32f769i_disco samples/hello_world
```

#### Button Sample

```
west build -p auto -b stm32f769i_disco samples/basic/button
```

#### Basic Thread Sample

```
west build -p auto -b stm32f769i_disco samples/basic/threads
```

#### Touchscreen Sample

```
west build -p auto -b stm32f769i_disco samples/drivers/kscan_touch/
```

### 3. Prueba básica de los sensores

### 4. Prueba básica de las funciones de red

#### Socket Echo Server Sample

#### MQTT Publisher Sample

### 5. Prueba básica con la plataforma ThingsBoard

[Documentación oficial de ThingsBoard Community Edition](https://thingsboard.io/docs/) -
[Repositorio oficial](https://github.com/thingsboard/thingsboard)

## Propuesta de práctica
