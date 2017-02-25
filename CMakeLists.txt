cmake_minimum_required (VERSION 3.1)
project (min-c)

set (CMAKE_CXX_STANDARD 11)

find_package (Threads)

include_directories(external)

add_library(min-c-logic
    src/Pass.cpp
    src/Pass.h
    src/PassManager.cpp
    src/PassManager.h
    src/ConsoleStatus.cpp
    src/ConsoleStatus.h
    src/DirectoryCopier.cpp
    src/DirectoryCopier.h
    src/MinCInvocation.cpp
    src/MinCInvocation.h
    src/PassRunner.cpp
    src/PassRunner.h
    src/ShellCommand.cpp
    src/ShellCommand.h
    src/ChildProcessDelegate.cpp
    src/ChildProcessDelegate.h
    src/Utils.cpp
    src/Utils.h
)
target_link_libraries(min-c-logic dl ${CMAKE_THREAD_LIBS_INIT})

# Logic tests
add_executable(ShellCommandTest src/test/ShellCommandTest.cpp)
target_link_libraries(ShellCommandTest min-c-logic)

# Main executable
add_executable(min-c
    src/main.cpp
)
target_link_libraries(min-c min-c-logic)

# Plugins
add_library(RemoveLinesPass SHARED plugins/RemoveLinesPass.cpp)
add_library(RemoveCharPass SHARED plugins/RemoveCharPass.cpp)
add_library(RemoveFilePass SHARED plugins/RemoveFilePass.cpp)
add_library(RunTopformflatPass SHARED plugins/RunTopformflatPass.cpp)
add_library(RunClangDeltaPass SHARED plugins/RunClangDeltaPass.cpp)
# Plugin tests
add_executable(GenericFilterTest plugins/test/GenericFilterTest.cpp plugins/GenericFilter.h)
add_test(GenericFilter GenericFilterTest)

add_executable(CommandRunningTest plugins/test/CommandRunningTest.cpp plugins/CommandRunning.h)
add_test(CommandRunning CommandRunningTest)



