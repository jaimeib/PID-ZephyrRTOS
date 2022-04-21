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
