cmake_minimum_required(VERSION 3.7)

# Jmeno projektu
project(ifj21c-test)

# -std=c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "-O2 -fcommon")
set(CMAKE_BUILD_TYPE Debug)

# Include hlavickovych souboru ve slozce include
# include_directories(include)

# Nastaveni promennych
set(COMPILER_LIB comp_lib)
set(TEST_SOURCE test_source)
set(SOURCE token.c token.h istring.c istring.h scanner.h scanner.c error.c error.h)

# Podslozky se CMakeLists.txt
add_subdirectory(googletest)

# Sestaveni testovaciho programu
add_executable(${PROJECT_NAME} ${TEST_SOURCE}.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${COMPILER_LIB} gtest gtest_main)

# Tvorba knihovny "libmath_lib.so"
add_library(${COMPILER_LIB} SHARED ${SOURCE})
