# Подключаем GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(lab3_tests
    marker_tests.cpp
    ../src/marker.cpp
    ../src/manager.cpp
)

target_include_directories(lab3_tests PRIVATE ../src)
target_link_libraries(lab3_tests gtest gtest_main)
add_test(NAME MarkerTests COMMAND lab3_tests)
