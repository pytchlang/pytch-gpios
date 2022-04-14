cmake_minimum_required(VERSION 3.18.0)

project(websocket-gpios)

add_compile_options(-Wall -Wextra -pedantic -Werror)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

################################################################################

add_library(websocketgpios
  Types.cpp
)

target_link_libraries(websocketgpios PUBLIC Threads::Threads)
target_compile_features(websocketgpios PUBLIC cxx_std_17)

################################################################################

add_executable(run-tests
  tests/test-main.cpp
  tests/test-Types.cpp
)

target_link_libraries(run-tests PUBLIC websocketgpios Threads::Threads)
target_compile_features(run-tests PUBLIC cxx_std_17)
