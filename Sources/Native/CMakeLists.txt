cmake_minimum_required(VERSION 3.23)
project(Native)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

file(GLOB XS2_SDK_FILES
        "XScript2/Frontend/*.cpp"
        "XScript2/Frontend/Parsers/*.cpp"
        "XScript2/Backend/*.cpp"
        "XScript2/Backend/Compiler/*.cpp"
        "XScript2/Executor/*.cpp"
        "XScript2/Share/*.cpp"
        "XScript2/Share/Exceptions/*.cpp"
        "XScript2/Core/*.cpp"
        "XScript2/Core/Serializatior/*.cpp"
        "XScript2/Core/Reader/*.cpp"
        "XScript2/NativeLibrary/*.cpp")

file(GLOB NET_SRC_FILES
        LibNetworking/Utils/*.cpp
        LibNetworking/Socket/*.cpp)

file(GLOB SRC_FILES
        Share/*.cpp
        library.cpp library.h)

add_library(socket SHARED ${NET_SRC_FILES} ${XS2_SDK_FILES} ${SRC_FILES})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../XPMBuildDir/NativeLibraries)

if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    TARGET_LINK_LIBRARIES(socket wsock32 ws2_32)
    set(CMAKE_EXE_LINKER_FLAGS "--static -lpthread")
endif ()