cmake_minimum_required(VERSION 3.20)

project(quant_pricing_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(quant
    src/instruments/european_option.cpp
    src/models/BlackScholes.cpp
    src/models/monte_carlo.cpp
)

target_include_directories(quant PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(option_example
    examples/option_example.cpp
)

target_link_libraries(option_example PRIVATE quant)

add_executable(black_scholes_tests
    tests/black_scholes_tests.cpp
)

target_link_libraries(black_scholes_tests PRIVATE quant)

add_executable(BS_MC_test
    tests/BS_MC_test.cpp
)

target_link_libraries(BS_MC_test PRIVATE quant)