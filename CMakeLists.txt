cmake_minimum_required (VERSION 3.0)
project (min-c)
add_executable(min-c
    src/main.cpp
    src/Pass.cpp
    src/Pass.h
    src/PassManager.cpp
    src/PassManager.h
    src/DirectoryCopier.cpp
    src/DirectoryCopier.h
)
target_link_libraries(min-c dl)

add_library(RemoveLinesPass SHARED plugins/RemoveLinesPass.cpp )
