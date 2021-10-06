cmake_minimum_required(VERSION 3.7)

# Jmeno projektu
project(ifj21c-test)

# -std=c++14
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include hlavickovych souboru ve slozce include
# include_directories(include)

# Nastaveni promennych
set(COMPILER_LIB comp_lib)
set(TEST_SOURCE test_source)
set(SOURCE istring.c istring.h scanner.h scanner.c)

# Podslozky se CMakeLists.txt
add_subdirectory(googletest)

# Tvorba spustitelneho "./test_lib"
# g++ -o ${TEST_SOURCE} ${TEST_SOURCE}.cpp -L. -l${MATH_LIB} -lgtest -lgtest_main
add_executable(${PROJECT_NAME} ${TEST_SOURCE}.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${COMPILER_LIB} gtest gtest_main)

# Tvorba knihovny "libmath_lib.so"
add_library(${COMPILER_LIB} SHARED ${SOURCE})
