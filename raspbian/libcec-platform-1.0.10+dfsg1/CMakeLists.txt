project(platform)

cmake_minimum_required(VERSION 2.8.9)
enable_language(CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Threads REQUIRED)
include(UseMultiArch.cmake)
include(CheckAtomic.cmake)

set(cec-platform_NAME cec-platform)
set(cec-platform_DESCRIPTION "libCEC Platform support library")
set(cec-platform_VERSION_MAJOR 1)
set(cec-platform_VERSION_MINOR 0)
set(cec-platform_VERSION_PATCH 10)

set(CMAKE_POSITION_INDEPENDENT_CODE on)

if(WIN32)
  set(PLAT_SOURCES src/windows/dlfcn-win32.cpp
                   src/windows/os-threads.cpp)
endif()

set(cec-platform_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include/cec-platform")
IF(WIN32)
  LIST(APPEND cec-platform_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include/cec-platform/windows")
ENDIF(WIN32)
set(cec-platform_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} fstrcmp)

if(NOT ${CORE_SYSTEM_NAME} STREQUAL "")
  if(${CORE_SYSTEM_NAME} STREQUAL "darwin" OR ${CORE_SYSTEM_NAME} STREQUAL "ios")
    list(APPEND cec-platform_LIBRARIES "-framework CoreVideo -framework IOKit")
  endif()
endif()

set(SOURCES
            src/util/StringUtils.cpp)

add_library(cec-platform ${SOURCES} ${PLAT_SOURCES})
target_link_libraries(cec-platform ${cec-platform_LIBRARIES})
set_target_properties(cec-platform PROPERTIES VERSION ${cec-platform_VERSION_MAJOR}.${cec-platform_VERSION_MINOR}.${cec-platform_VERSION_PATCH}
                                              SOVERSION ${cec-platform_VERSION_MAJOR})

install(TARGETS cec-platform DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES src/os.h DESTINATION include/cec-platform)
IF(WIN32)
  INSTALL(FILES src/windows/dlfcn-win32.h
                src/windows/os-socket.h
                src/windows/os-threads.h
                src/windows/os-types.h
          DESTINATION include/cec-platform/windows)
ELSE(WIN32)
  install(FILES src/posix/os-socket.h
                src/posix/os-threads.h
                src/posix/os-types.h
          DESTINATION include/cec-platform/posix)
ENDIF(WIN32)
install(FILES src/sockets/cdevsocket.h
              src/sockets/socket.h
              src/sockets/tcp.h
        DESTINATION include/cec-platform/sockets)
install(FILES src/threads/atomics.h
              src/threads/mutex.h
              src/threads/threads.h
        DESTINATION include/cec-platform/threads)
install(FILES src/util/atomic.h
              src/util/buffer.h
              src/util/StringUtils.h
              src/util/StdString.h
              src/util/timeutils.h
              src/util/util.h
        DESTINATION include/cec-platform/util)

IF(NOT WIN32)
  # Pkgconfig
  include(PkgConfigHandler.cmake)
  configure_pc_file(cec-platform platform.pc.in
                                 cec-platform.pc
                                 ${CMAKE_INSTALL_PREFIX}
                                 ${CMAKE_INSTALL_LIBDIR}
                                 ${CMAKE_INSTALL_PREFIX}/include)

  install(FILES ${CMAKE_BINARY_DIR}/cec-platform.pc
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
ENDIF(NOT WIN32)

# config mode
configure_file (platform-config.cmake.in
                cec-platform-config.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/cec-platform-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cec-platform)

