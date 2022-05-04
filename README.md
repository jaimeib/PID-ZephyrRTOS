# Práctica Zephyr-RTOS con STM32F769-Discovery-Kit

**Práctica de colaboración en el Dpto. Ingeniería Informática y Electrónica de la Universidad de Cantabria**

> _Implantación de plataforma docente para sistemas empotrados y tecnologías industria 4.0_

## Características del estándar POSIX que aún no son compatibles:

-   `pthread_attr_setinheritsched` & `PTHREAD_EXPLICIT_SCHED` para la planificación de hilos (por defecto, la planificación es implicita).

    > GitHub Issue [#4](https://github.com/jaimeib/Practica-PID-ZephyrRTOS/issues/4)

-   `usleep` para la espera de microsegundos. La llamada de esta función no está implementada como en el estandar POSIX. Hace uso de internamente de la llamada `k_msleep` de la API de Threads de Kernel suministrada por Zephyr.

    > GitHub Issue [#5](https://github.com/jaimeib/Practica-PID-ZephyrRTOS/issues/5)

-   `clock_nanosleep` para realizar esperas absolutas a partir de un clock (CLOCK_MONOTONIC). Si hay funciones para leer los valores de los clocks, pero no para realizar las esperas. Hay llamadas similares pero no siguen el mismo estándar.

    > GitHub Issue [#6](https://github.com/jaimeib/Practica-PID-ZephyrRTOS/issues/6)
