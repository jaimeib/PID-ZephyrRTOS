# Propuesta de Práctica

## Introducción

## Descripción de los threads:

### - Thread LED:

Este thread se encarga de encender y apagar el LED 1 (color rojo) de usuario de la placa, cada segundo.

### - Thread sensor de luz:

Este thread se encarga de leer el valor del sensor de luz desde conectado al pin A0 de Arduino.

### - Thread sensor de temperatura interno:

Este thread se encarga de leer el valor del sensor de temperatura interno de la placa.

### - Thread supervisor:

Este thread se encarga de obtener las lecturas de threads de los sensores, y en función de unos valores umbrales definidos, encender o apagar los LED de control (LEDs 2 y 3 de usuario de color verde).

### - Thread de publicación MQTT:

Este thread se encarga de publicar los datos de los sensores, utilizando el protocolo MQTT. Dicho thread deberá configurar y establecer la conexión con el broker MQTT y el cliente.

## Configuración de los threads:

Hay que tener en cuenta la que el stack de los threads debe ser de al menos 2048 bytes, ya que con un valor potencia de 2 y múltiplo de 512 bytes menor, se produce un error de stack overflow en el thread de publicación.

## Sincronización de los threads:

-   Los threads de LED, sensor de luz y sensor de temperatura interno deben iniciarse una vez los threads de publicación MQTT y supervisor estén esperando resultados, para no perder medidas.

-   Los threads que manejan los sensores deben sincronizarse para escribir los resultados en la estructura definida del tipo `thread_result_t`. Esta estructura tiene 2 campos: `type` para indicar el tipo de sensor que escribe la medida, representado por un tipo enumerado, y `value` para escribir el dato. Se avisan al thread supervisor y al thread de publicación, al mismo tiempo, que se han escrito los datos, para que los procesen.

-   Los threads supervisor y publicación MQTT deben leer los campos de la estructura en exclusión mutua, para evitar que algún dato se cambie mientras se lee. Cuando se leen los valores, se copian en una estructura privada del thread, con la cual ya operar sin que tengan acceso el resto de threads.

## Estructura del proyecto:

```bash
Practica
├── CMakeLists.txt
├── include
│   ├── internal_temp.h
│   ├── leds.h
│   ├── light_sensor.h
│   ├── main.h
│   ├── mqtt_publisher.h
│   └── supervisor.h
├── Kconfig
├── prj.conf
└── src
    ├── internal_temp.c
    ├── leds.c
    ├── light_sensor.c
    ├── main.c
    ├── mqtt_publisher.c
    └── supervisor.c
```

## Soporte POSIX en Zephyr RTOS:

> [Soporte de POSIX en Zephyr RTOS](https://docs.zephyrproject.org/latest/services/portability/posix.html)

### Características del estándar POSIX que aún no son compatibles:

-   `pthread_attr_setinheritsched` & `PTHREAD_EXPLICIT_SCHED` para la planificación de hilos (por defecto, la planificación es implicita).

    > GitHub Issue [#4](https://github.com/jaimeib/Practica-PID-ZephyrRTOS/issues/4)

-   `usleep` para la espera de microsegundos. La llamada de esta función no está implementada como en el estandar POSIX. Hace uso de internamente de la llamada `k_msleep` de la API de Threads de Kernel suministrada por Zephyr.

    > GitHub Issue [#5](https://github.com/jaimeib/Practica-PID-ZephyrRTOS/issues/5)

-   `clock_nanosleep` para realizar esperas absolutas a partir de un clock (CLOCK_MONOTONIC). Si hay funciones para leer los valores de los clocks, pero no para realizar las esperas. Hay llamadas similares pero no siguen el mismo estándar.

    > GitHub Issue [#6](https://github.com/jaimeib/Practica-PID-ZephyrRTOS/issues/6)

## Plataforma ThingsBoard usando Docker en Linux:

### Requisitos:

-   Es necesario tener instalado y configurado [Docker CE](https://docs.docker.com/engine/install/) en su equipo.
-   Es necesario tener instalado [Docker Compose](https://docs.docker.com/compose/install/) en su equipo.

### Instalación:

Se ha seguido la [guía de instalación de Things en Docker (Linux or Mac OS)](https://thingsboard.io/docs/user-guide/install/docker/?ubuntuThingsboardQueue=inmemory) proporcionada en la web oficial de ThingsBoard.

> En este caso, nos basta con lanzar un servidor que utilice PostgreSQL como base de datos. Utilizaremos el sistema de colas in-memory de ThingsBoard.

Crearemos un [fichero docker-compose.yml](ThingsBoard/Docker/docker-compose.yml) que contiene toda la configuración necesaria para el contenedor de Docker, con el servicio de ThingsBoard.

A continuación, crearemos los directorios para almacenar los datos y los ficheros de log del contenedor. Actualizamos el propietario de los ficheros.

```bash
mkdir -p ~/.mytb-data && sudo chown -R 799:799 ~/.mytb-data
mkdir -p ~/.mytb-logs && sudo chown -R 799:799 ~/.mytb-logs
```

> El path de los directorios se ha especificado anteriormente en el fichero de configuración.

Por último, lanzamos el contenedor de Docker, desde el directorio de este el fichero de configuración `docker-compose.yml`.

```bash
docker compose pull
docker compose up
```

Para parar el contenedor, ejecutamos:

```bash
docker compose down
```

### Configuración de ThingsBoard:

En el navegador nos conectamos al servidor `http://localhost:8080` y nos autenticamos con el usuario `tenant@thingsboard.org` y la contraseña `tenant`, para configurar los paneles de los dispositivos.

Añadimos en el apartado de **Perfiles de Dispositivos** el perfil [stm32f769_profile.json](ThingsBoard/Plantillas/stm32f769_profile.json) proporcionado y creamos un dispositivo usando dicho perfil.

> Es necesario modificar la identificación del dispositivo, ya que por defecto se genera un Access Token aleatorio. En este caso, se utilizará el identificador `zephyr_publisher`, definido en el fichero [mqtt_publisher.h](/Practica/src/include/mqtt_publisher.h), definido como `MQTT_CLIENTID`.

Importamos el dashboard [ThingsBoard_dashboard.json](ThingsBoard/Plantillas/ThingsBoard_dashboard.json) proporcionado en el área de **Paneles**, y añadimos el dispositivo creado anteriormente, en el menú de **Alias de Dispositivos**.
