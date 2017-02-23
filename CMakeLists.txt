cmake_minimum_required (VERSION 3.1)
project (min-c)

set (CMAKE_CXX_STANDARD 11)

find_package (Threads)


add_executable(min-c
    src/main.cpp
    src/Pass.cpp
    src/Pass.h
    src/PassManager.cpp
    src/PassManager.h
    src/DirectoryCopier.cpp
    src/DirectoryCopier.h
    src/MinCInvocation.cpp
    src/MinCInvocation.h
    src/PassRunner.cpp
    src/PassRunner.h
)
target_link_libraries(min-c dl ${CMAKE_THREAD_LIBS_INIT})

add_library(RemoveLinesPass SHARED plugins/RemoveLinesPass.cpp )
add_library(RemoeCharPass SHARED plugins/RemoveCharPass.cpp )
