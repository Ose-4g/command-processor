cmake_minimum_required(VERSION 3.26)
project(commandprocessor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get all .cpp
file(GLOB ALL_CPP_FILES *.cpp)

# Remove test files (*.t.cpp)
foreach(FILE ${ALL_CPP_FILES})
    if(NOT FILE MATCHES "\\.t\\.cpp$")
        list(APPEND CPP_FILES ${FILE})
    endif()
endforeach()

add_executable(commandprocessor ${CPP_FILES})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(gtest_capture no)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Remove main files (*.m.cpp)
foreach(FILE ${ALL_CPP_FILES})
    if(NOT FILE MATCHES "\\.m\\.cpp$")
        list(APPEND TEST_FILES ${FILE})
    endif()
endforeach()

add_executable(
  commandprocessortest
  ${TEST_FILES}
)

target_link_libraries(
  commandprocessortest
  gtest
  gmock
)

include(GoogleTest)
gtest_discover_tests(commandprocessortest)