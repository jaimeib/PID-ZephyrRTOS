cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(Practica-Monitorizacion VERSION 1.0.0 LANGUAGES C)

FILE(GLOB app_sources src/*.c)
target_include_directories(app PRIVATE ./include)
target_include_directories(app PRIVATE $ENV{ZEPHYR_BASE})
target_sources(app PRIVATE ${app_sources})
