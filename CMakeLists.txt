cmake_minimum_required(VERSION 3.24)

option(W_CORE_BUILD_WTC640 "Build thermal-core Gui module - WWidgets and property-widgets mapping" OFF)
option(W_CORE_BUILD_WITH_EBUS "Build thermal-core with eBus support" OFF)
option(W_DEBUG_SYMBOLS_IN_RELEASE "Build with debug symbols in release for debugging" OFF)
option(W_BUILD_DEPENDENCIES_WITH_CONAN "Build dependencies using conan" OFF)
option(W_BUILD_EXAMPLE "Build example" OFF)
option(W_BUILD_TEST "Build test" OFF)

project(weomcpp VERSION 0.1.0 LANGUAGES CXX)

option(W_BUILD_WITH_AMENT "Build and export the SDK as an ament package" ON)
if(W_BUILD_WITH_AMENT)
    find_package(ament_cmake REQUIRED)
endif()

if(W_BUILD_DEPENDENCIES_WITH_CONAN)
    include(conan.cmake)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(WARNING "CMAKE_BUILD_TYPE was not set, defaulting to 'Release'")
endif()

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0A00 -DBOOST_USE_WINAPI_VERSION=0x0A00")
    if(CMAKE_BUILD_TYPE STREQUAL "Release" AND W_DEBUG_SYMBOLS_IN_RELEASE)
        set(CMAKE_CXX_FLAGS_RELEASE "/Zi /Od /Ob0 /Oy-")
        set(CMAKE_C_FLAGS_RELEASE "/Zi /Od /Ob0 /Oy-")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG")
    endif()
endif()

add_subdirectory(core)
add_subdirectory(wtc640)

if(W_CORE_BUILD_WITH_EBUS)
    add_subdirectory(ebus-plugin)
endif()



if(W_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(W_BUILD_TEST)
    add_subdirectory(tests)
endif()

install(TARGETS Core WTC640
    EXPORT export_weomcpp
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)
install(DIRECTORY core/include/ wtc640/include/ DESTINATION include)
install(FILES LICENSE DESTINATION share/${PROJECT_NAME})

if(W_BUILD_WITH_AMENT)
    ament_export_targets(export_weomcpp HAS_LIBRARY_TARGET NAMESPACE ThermalCore::)
    ament_package(CONFIG_EXTRAS cmake/weomcpp-dependencies.cmake.in)
endif()
