cmake_minimum_required(VERSION 3.18.0)

project(websocket-gpios)

add_compile_options(-Wall -Wextra -pedantic -Werror)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

########################################################################
#
# Determine whether to include code for driving actual RPi hardware.

set(RPI_SOURCES )
set(RPI_LIBRARIES )

if(DEFINED ENABLE_PIGPIO)
  message("-- Enabling pigpio")
  list(APPEND RPI_SOURCES PiGpioArray.cpp)
  list(APPEND RPI_LIBRARIES pigpio)
  add_compile_definitions(ENABLE_PIGPIO=1)
else()
  message("-- Omitting pigpio")
  add_compile_definitions(ENABLE_PIGPIO=0)
endif()

# Testing with DEFINED does not count as "using" the variable, so
# do so now:
set(quietCMake ${ENABLE_PIGPIO})

################################################################################

add_library(websocketgpios
  Types.cpp
  StubGpioArray.cpp
  LoopbackGpioArray.cpp
  GpioJsonInterface.cpp
  GpioInterfaceBroker.cpp
  ClientSession.cpp
  WebSocketListener.cpp
  ${RPI_SOURCES}
)

target_link_libraries(websocketgpios PUBLIC Threads::Threads ${RPI_LIBRARIES})
target_compile_features(websocketgpios PUBLIC cxx_std_17)

################################################################################

add_executable(run-tests
  tests/test-main.cpp
  tests/test-Types.cpp
  tests/test-StubGpioArray.cpp
  tests/test-loopback.cpp
  tests/test-json.cpp
  tests/test-GpioInterfaceBroker.cpp
)

target_link_libraries(run-tests PUBLIC websocketgpios Threads::Threads)
target_compile_features(run-tests PUBLIC cxx_std_17)

################################################################################

add_executable(websocket-gpio-server
  WebSocketServer.cpp
)

target_link_libraries(websocket-gpio-server PUBLIC
  websocketgpios
  Threads::Threads
)

target_compile_features(websocket-gpio-server PUBLIC cxx_std_17)
