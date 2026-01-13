cmake_minimum_required(VERSION 3.21)

# Set the default build type to Release because we want to benchmark optimized
# code.
if(NOT CMAKE_BUILD_TYPE)
  set(none "None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used)")
  set(other "Debug Release RelWithDebInfo MinSizeRel")
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Choose the type of build, options are: ${none} ${other}.")
endif()

project(JsonBenchmarkCpp C CXX)

include(ExternalProject)

option(USE_SANITIZERS_FOR_DEBUG "Use Sanitizers for Debug build" ON)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("Using C++${CMAKE_CXX_STANDARD} standard")

if(UNIX)
  set(CMAKE_SKIP_RPATH OFF)
  set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
  set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib")
endif()

foreach(
  var_name
  CMAKE_C_COMPILER
  CMAKE_C_FLAGS
  CMAKE_CXX_COMPILER
  CMAKE_CXX_STANDARD
  CMAKE_CXX_FLAGS
  CMAKE_SHARED_LINKER_FLAGS
  CMAKE_BUILD_TYPE)
  if(DEFINED ${var_name})
    list(APPEND EXT_PROJECT_CMAKE_ARGS "-D${var_name}=${${var_name}}")
  endif()
endforeach()

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
set(CONFIG_SUBDIR $<$<BOOL:${IS_MULTI_CONFIG}>:$<CONFIG>>)

set(UXS_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/uxs/${CONFIG_SUBDIR})
set(UXS_INCLUDE_DIR ${UXS_INSTALL_DIR}/include)
set(UXS_LIBRARY_NAME $<IF:$<BOOL:${WIN32}>,uxs.lib,libuxs.so>)
set(UXS_LIBRARY ${UXS_INSTALL_DIR}/lib/${UXS_LIBRARY_NAME})
ExternalProject_Add(
  uxs
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/uxs
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/uxs
  BUILD_BYPRODUCTS ${UXS_LIBRARY}
  CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${UXS_INSTALL_DIR}
  CMAKE_ARGS -DUSE_ZLIB=Off #
             -DUSE_LIBZIP=Off #
             -DUSE_SANITIZERS_FOR_DEBUG=${USE_SANITIZERS_FOR_DEBUG} #
             ${EXT_PROJECT_CMAKE_ARGS})

if(WIN32)
  install(
    DIRECTORY ${UXS_INSTALL_DIR}/bin
    DESTINATION .
    COMPONENT binary)
elseif(UNIX)
  install(
    DIRECTORY ${UXS_INSTALL_DIR}/lib
    DESTINATION .
    COMPONENT library)
endif()

if(MSVC)
  add_compile_options(/Zc:__cplusplus /utf-8)
endif()

if(USE_SANITIZERS_FOR_DEBUG)
  message("Using sanitizers for Debug build")
  if(MSVC)
    add_compile_options($<$<CONFIG:Debug>:/fsanitize=address>)
  else()
    add_compile_options($<$<CONFIG:Debug>:-fsanitize=address,undefined,leak>)
    add_link_options($<$<CONFIG:Debug>:-fsanitize=address,undefined,leak>)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_link_options($<$<CONFIG:Debug>:-shared-libsan>)
    endif()
  endif()
endif()

add_executable(
  JsonBenchmarkCpp
  main.cpp
  libs/jsoncpp/src/lib_json/json_value.cpp
  libs/jsoncpp/src/lib_json/json_reader.cpp
  libs/jsoncpp/src/lib_json/json_writer.cpp
  libs/libjson/_internal/Source/internalJSONNode.cpp
  libs/libjson/_internal/Source/JSONAllocator.cpp
  libs/libjson/_internal/Source/JSONChildren.cpp
  libs/libjson/_internal/Source/JSONIterators.cpp
  libs/libjson/_internal/Source/JSONMemory.cpp
  libs/libjson/_internal/Source/JSONNode.cpp
  libs/libjson/_internal/Source/JSONNode_Mutex.cpp
  libs/libjson/_internal/Source/JSONPreparse.cpp
  libs/libjson/_internal/Source/JSONStream.cpp
  libs/libjson/_internal/Source/JSONValidator.cpp
  libs/libjson/_internal/Source/JSONWorker.cpp
  libs/libjson/_internal/Source/JSONWriter.cpp
  libs/libjson/_internal/Source/libjson.cpp
  libs/json_spirit/json_spirit/json_spirit_reader.cpp
  libs/json_spirit/json_spirit/json_spirit_value.cpp
  libs/json_spirit/json_spirit/json_spirit_writer.cpp
  libs/json-parser/json-parser/json.c)

add_dependencies(JsonBenchmarkCpp uxs)

target_include_directories(
  JsonBenchmarkCpp
  PRIVATE libs/rapidjson/include
          libs/nlohmann/single_include
          libs/jsoncpp/include
          libs/libjson
          libs/cajun/include
          libs/json_spirit/json_spirit
          libs/json-parser
          libs/avery
          ${UXS_INCLUDE_DIR})

target_link_libraries(JsonBenchmarkCpp PRIVATE ${UXS_LIBRARY})

install(TARGETS JsonBenchmarkCpp RUNTIME DESTINATION bin COMPONENT binary)
