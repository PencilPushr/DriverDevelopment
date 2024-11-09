cmake_minimum_required(VERSION 3.20)

project(KernelDriver)

## Set WDK root directory (update the path as needed)
#if(NOT DEFINED ENV{WDK_DIR})
#    #message(FATAL_ERROR "Windows Driver Kit directory not found. Please install the WDK.")
#    set(WDK_ROOT "C:/Program Files (x86)/Windows Kits/10" CACHE STRING "Path to Windows Driver Kit")
#endif()

# Verify WDK_ROOT exists
set(WDK_ROOT "C:/Program Files (x86)/Windows Kits/10" CACHE STRING "Path to Windows Driver Kit")
if(NOT EXISTS "${WDK_ROOT}")
    message(FATAL_ERROR "Windows Driver Kit directory not found at ${WDK_ROOT}. Please install the WDK.")
endif()

# Windows configuration / Specify that we're building a driver
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10.0)
set(ARCH x64)  # Change to x86 for 32-bit drivers

# Find the latest WDK version
file(GLOB WDK_INCLUDE_DIRS RELATIVE "${WDK_ROOT}/Include" "${WDK_ROOT}/Include/*")

# Filter out directories that match the version pattern (e.g., "10.0.22000.0")
set(VERSION_DIRS)
foreach(dir ${WDK_INCLUDE_DIRS})
    if(dir MATCHES "^10\\.[0-9]+\\.[0-9]+\\.[0-9]+$")
        list(APPEND VERSION_DIRS ${dir})
    endif()
endforeach()

# Ensure we found at least one version directory
if(NOT VERSION_DIRS)
    message(FATAL_ERROR "No WDK version directories found in ${WDK_ROOT}/Include.")
endif()

# Correct version
list(SORT VERSION_DIRS)
list(REVERSE VERSION_DIRS)  # Reverse to get highest version first
list(GET VERSION_DIRS 0 WDK_VERSION)

message(STATUS "Using WDK version: ${WDK_VERSION}")

# Include directories
include_directories(
    "${WDK_ROOT}/Include/${WDK_VERSION}/km"
    "${WDK_ROOT}/Include/${WDK_VERSION}/shared"
)

# Library directories
link_directories(
    "${WDK_ROOT}/Lib/${WDK_VERSION}/km/${ARCH}"
)

# Add source files
add_executable(${PROJECT_NAME}
    
 "src/VTPTranslator.h" "src/VADEnum.h" "Driver.cpp" "src/DriverDeviceControl.cpp")

# Set driver-specific properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "KernelDriver"
    SUFFIX ".sys"
    LINK_FLAGS "/DRIVER:WDM /SUBSYSTEM:NATIVE /ENTRY:DriverEntry"
)

# Link against required libraries
target_link_libraries(${PROJECT_NAME}
    ntoskrnl.lib
    #hal.lib
    #wmilib.lib
)

# Compiler definitions based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DBG=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
endif()

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zl /GR- /EHsc- /GS-")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zl /GR- /EHsc- /GS-")

# Disable specific warnings
if(MSVC)
    add_compile_options(
        /wd4100   # Unreferenced formal parameter
        /wd4201   # Nameless struct/union
        /wd4214   # Bit field types other than int
        /wd4324   # Structure was padded
    )
endif()