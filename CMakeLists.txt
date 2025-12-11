cmake_minimum_required(VERSION 3.18)
project(dwm-ma VERSION 0.0.1 LANGUAGES C)

add_library(dwm-ma STATIC dwm_ma.c ma_config.c)
target_include_directories(dwm-ma PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET dwm-ma PROPERTY C_STANDARD 11)