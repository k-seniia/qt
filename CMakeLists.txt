cmake_minimum_required(VERSION 3.16)
project(MultiCriteriaOptimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Автоматична генерація MOC/UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Пошук модуля Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(MultiCriteriaOptimization
    main.cpp
    mainwindow.cpp
    mainwindow.h

    doubleitemdelegate.h doubleitemdelegate.cpp
)

target_link_libraries(MultiCriteriaOptimization Qt6::Widgets)
