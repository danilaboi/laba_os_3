cmake_minimum_required(VERSION 3.20)
project(laba_os_3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/MP)
endif()

# Главный исполняемый файл
add_executable(lab3_main
    src/main.cpp
    src/marker.cpp
    src/manager.cpp
)

target_include_directories(lab3_main PRIVATE src)

# Добавляем подпроект с тестами
add_subdirectory(tests)
enable_testing()
