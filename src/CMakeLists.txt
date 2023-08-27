cmake_minimum_required(VERSION 3.5)

project(DNSCache LANGUAGES CXX)

find_package(Boost 1.74.0 REQUIRED)
find_package(Threads REQUIRED)

add_executable("${PROJECT_NAME}" main.cpp
   dns_cache.cpp
   dns_cache.h)

target_include_directories("${PROJECT_NAME}" PRIVATE
  ${Boost_INCLUDE_DIRS})
target_link_libraries("${PROJECT_NAME}"
  ${Boost_LIBRARIES}
  Threads::Threads)
