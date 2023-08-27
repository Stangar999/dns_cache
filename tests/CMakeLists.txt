cmake_minimum_required(VERSION 3.5)

project(MonoTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.74.0 REQUIRED)
find_package(GTest REQUIRED) # ver 1.10.0
find_package(Threads REQUIRED)

file(GLOB TESTS_SRCS
    ./*.cpp
    ./*.h
)

file(GLOB PROJ_SRCS
    ${CMAKE_HOME_DIRECTORY}/src/dns_cache.cpp
    ${CMAKE_HOME_DIRECTORY}/src/*.h
)

add_executable("${PROJECT_NAME}"
  ${TESTS_SRCS}
  ${PROJ_SRCS}
)

target_include_directories("${PROJECT_NAME}" PRIVATE
  ${Boost_INCLUDE_DIRS}
  ${GTEST_INCLUDE_DIRS}
  ${CMAKE_HOME_DIRECTORY}/src )

target_link_libraries("${PROJECT_NAME}"
  ${Boost_LIBRARIES}
  ${GTEST_BOTH_LIBRARIES}
  Threads::Threads)

add_test("${PROJECT_NAME}" test)

add_custom_target("run_${PROJECT_NAME}"
  $<TARGET_FILE:${PROJECT_NAME}>
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
