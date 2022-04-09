# Práctica Zephyr Project OS con STM32F769 Discovery Kit

**Práctica de colaboración en el Dpto. Ingeniería Informática y Electrónica de la Universidad de Cantabria**

_Implantación de plataforma docente para sistemas empotrados y tecnologías industria 4.0_

### Zephyr Project 

[Zephyr Project](https://zephyrproject.org/) es un pequeño sistema operativo de tiempo real (RTOS) para dispositivos con recursos limitados y embebidos (IoT) que admite múltiples arquitecturas. Se publica bajo la Licencia Apache 2.0. Zephyr incluye un kernel y todos los componentes y bibliotecas, controladores de dispositivos, pilas de protocolos, sistemas de archivos y actualizaciones de firmware necesarios para desarrollar una aplicación software completa.

### Placa STM32F769 Discovery Kit

El [STM32F7 discovery kit](https://www.st.com/en/evaluation-tools/32f769idiscovery.html) permite a los usuarios desarrollar y compartir aplicaciones con los microcontroladores de la serie STM32F7 basados en el núcleo [ARM®Cortex®-M7](https://www.arm.com/products/silicon-ip-cpu/cortex-m/cortex-m7).

Permite una amplia diversidad de aplicaciones que se benefician de las características de audio, soporte multisensor, gráficos, seguridad, video y conectividad de alta velocidad. El soporte de conectividad Arduino™ proporciona capacidades de expansión ilimitadas con una gran variedad de placas complementarias especializadas, como la [Grove Base Shield V2.0 for Arduino](https://www.seeedstudio.com/Base-Shield-V2.html).

## Guía de inicio

### 1. Instalación del sistema operativo

Zephyr se ha instalado correctamente en Ubuntu 20.04 LTS siguiendo las indicaciones de la documentación oficial, para la versión sobre el entorno virtual en Python.

[Documentación oficial del sistema operativo Zephyr Project de Linux Fundation](https://docs.zephyrproject.org/latest/index.html) - 
[Repositorio oficial](https://github.com/zephyrproject-rtos/zephyr)

### 2. Prueba básica con la placa STM32F769 Discovery Kit

#### Blinky Sample
```
cd ~/zephyrproject/zephyr
west build -p auto -b stm32f769i_disco samples/basic/blinky
west flash
```
#### Hello World! Sample
```
cd ~/zephyrproject/zephyr
west build -p auto -b stm32f769i_disco samples/hello_world
west flash
```

#### Button Sample
```
cd ~/zephyrproject/zephyr
west build -p auto -b stm32f769i_disco samples/basic/button
west flash
```

#### Basic Thread Sample
```
cd ~/zephyrproject/zephyr
west build -p auto -b stm32f769i_disco samples/basic/threads
west flash
```

#### Touchscreen Sample
```
cd ~/zephyrproject/zephyr
west build -p auto -b stm32f769i_disco samples/drivers/kscan_touch/
west flash
```

### 3. Prueba básica de los sensores

### 4. Prueba básica de las funciones de red

#### Socket Echo Server Sample

#### MQTT Publisher Sample

## Propuesta de práctica
