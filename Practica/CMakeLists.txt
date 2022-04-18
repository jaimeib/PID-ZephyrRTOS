cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(Practica)

FILE(GLOB app_sources src/*.c)
target_include_directories(app PRIVATE $ENV{ZEPHYR_BASE}/include/posix)
target_sources(app PRIVATE ${app_sources})

