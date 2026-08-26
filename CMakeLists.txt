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

include(FetchContent)
include(GNUInstallDirs)

option(OPTION_USE_LIBCPP "Use LibC++" OFF)

set(CMAKE_CXX_STANDARD
    20
    CACHE STRING "Default C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message("Using C++${CMAKE_CXX_STANDARD} standard")

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

if(UNIX)
  set(CMAKE_SKIP_RPATH OFF)
  set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
  set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

if(NOT MSVC
   AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
   AND OPTION_USE_LIBCPP)
  message("Using libc++")
  add_compile_options(-stdlib=libc++)
  add_link_options(-stdlib=libc++)
endif()

block()
set(UXS_USE_ZLIB OFF)
set(UXS_USE_LIBZIP OFF)
set(UXS_DO_INSTALL_SDK OFF)
FetchContent_Declare(uxs SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/uxs)
FetchContent_MakeAvailable(uxs)
endblock()

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

target_compile_options(
  JsonBenchmarkCpp PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus /utf-8>)

target_include_directories(
  JsonBenchmarkCpp
  PRIVATE libs/rapidjson/include
          libs/nlohmann/single_include
          libs/jsoncpp/include
          libs/libjson
          libs/cajun/include
          libs/json_spirit/json_spirit
          libs/json-parser
          libs/avery)

target_link_libraries(JsonBenchmarkCpp PRIVATE UXS::UXS)

install(TARGETS JsonBenchmarkCpp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                                         COMPONENT binary)
