# Propuesta de Práctica

[Soporte de POSIX en Zephyr RTOS](https://docs.zephyrproject.org/latest/services/portability/posix.html)

## Descripción de los threads:

### - Thread LED:

Este thread se encarga de encender y apagar el LED 1 de usuario de la placa, cada segundo.

### - Thread sensor de luz:

Este thread se encarga de leer el valor del sensor de luz desde conectado al pin A0 de Arduino.

### - Thread sensor de temperatura interno:

Este thread se encarga de leer el valor del sensor de temperatura interno de la placa.

### - Thread supervisor:

Este thread se encarga de obtener las lecturas de threads de los sensores

### - Thread de publicación MQTT:

Este thread se encarga de publicar los datos de los sensores, utilizando el protocolo MQTT.

## Configuración de los threads:

Hay que tener en cuenta la que el stack de los threads debe ser de al menos 2048 bytes, ya que con un valor multiplo de 512 bytes menor, se produce un error de stack overflow en el thread de publicación.

## Plataforma ThingsBoard usando Docker en Linux:

### Prerequisitos:

-   Es necesario tener instalado y configurado [Docker CE](https://docs.docker.com/engine/install/) en su equipo.
-   Es necesario tener instalado [Docker Compose](https://docs.docker.com/compose/install/) en su equipo.

### Instalación:

[Guía de instalación de Things en Docker (Linux or Mac OS)](https://thingsboard.io/docs/user-guide/install/docker/?ubuntuThingsboardQueue=inmemory)

En este caso, nos basta con lanzar un servidor que utilice PostgreSQL como base de datos. Utilizaremos el sistema de colas in-memory de ThingsBoard.

Create docker compose file for ThingsBoard queue service:

[Fichero de configuración del contenedor Docker](ThingsBoard/Docker/docker-compose.yml)

```bash
mkdir -p ~/.mytb-data && sudo chown -R 799:799 ~/.mytb-data
mkdir -p ~/.mytb-logs && sudo chown -R 799:799 ~/.mytb-logs
```

```bash
docker compose pull
docker compose up
```

### Configuración de ThingsBoard:

En el navegador nos conectamos al servidor `http://localhost:8080` y nos autenticamos con el usuario `tenant@thingsboard.org` y la contraseña `tenant`, para configurar los paneles de los dispositivos.
