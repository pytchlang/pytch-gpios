cmake_minimum_required(VERSION 3.18.0)

project(websocket-gpios)

add_compile_options(-Wall -Wextra -pedantic -Werror)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
