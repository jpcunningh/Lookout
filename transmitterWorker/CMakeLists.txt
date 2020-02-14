cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)
project (transmitterWorker LANGUAGES C CXX)

FIND_PACKAGE (Threads REQUIRED)
FIND_PACKAGE (PkgConfig REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED True)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -std=c++11 -Wextra -fno-omit-frame-pointer -DDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=c++11")

set (tinyb_PROJECT_DIR ~/src/tinyb)

set (tinyb_LIB_INCLUDE_DIRS
  ${tinyb_PROJECT_DIR}/api
  ${tinyb_PROJECT_DIR}/api/tinyb
  ${tinyb_PROJECT_DIR}/include
)

link_directories(
  ${tinyb_PROJECT_DIR}/build/src
)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${tinyb_LIB_INCLUDE_DIRS}
  ${GLIB2_INCLUDE_DIRS}
  ${GIO_INCLUDE_DIRS}
  ${GIO-UNIX_INCLUDE_DIRS}
)

set (transmitterWorker_SRCS
  ${PROJECT_SOURCE_DIR}/authChallengeRxMessage.cpp
  ${PROJECT_SOURCE_DIR}/authChallengeTxMessage.cpp
  ${PROJECT_SOURCE_DIR}/authenticator.cpp
  ${PROJECT_SOURCE_DIR}/authRequestTxMessage.cpp
  ${PROJECT_SOURCE_DIR}/authStatusRxMessage.cpp
  ${PROJECT_SOURCE_DIR}/bluetoothMessage.cpp
  ${PROJECT_SOURCE_DIR}/encryptor.cpp
  ${PROJECT_SOURCE_DIR}/uuidv4.cpp
  ${PROJECT_SOURCE_DIR}/transmitterWorker.cpp
)

set (stub_SRCS
  ${PROJECT_SOURCE_DIR}/authChallengeRxMessage.cpp
  ${PROJECT_SOURCE_DIR}/authChallengeTxMessage.cpp
  ${PROJECT_SOURCE_DIR}/authenticator.cpp
  ${PROJECT_SOURCE_DIR}/authRequestTxMessage.cpp
  ${PROJECT_SOURCE_DIR}/authStatusRxMessage.cpp
  ${PROJECT_SOURCE_DIR}/bluetoothMessage.cpp
  ${PROJECT_SOURCE_DIR}/encryptor.cpp
  ${PROJECT_SOURCE_DIR}/uuidv4.cpp
  ${PROJECT_SOURCE_DIR}/stub.cpp
)

add_executable (transmitterWorker ${transmitterWorker_SRCS})
set_target_properties(transmitterWorker
    PROPERTIES
    CXX_STANDARD 11)

add_executable (stub ${stub_SRCS})
set_target_properties(stub
    PROPERTIES
    CXX_STANDARD 11)

include_directories(${tinyb_PROJECT_DIR}/api)

target_link_libraries (transmitterWorker tinyb cryptopp)

target_link_libraries (stub tinyb cryptopp)

