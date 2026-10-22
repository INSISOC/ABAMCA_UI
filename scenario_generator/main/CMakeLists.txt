# Regular CMake project

#Change back - just for testing
add_executable(${projectName} main_test_input.cpp)

# Add required libraries and include directories
#target_include_directories(${projectName} PRIVATE "." "include" $ENV{CADMIUM})
target_include_directories(${projectName} PRIVATE "." "include" "~/cadmium_v2/include/")

target_compile_options(${projectName} PUBLIC -std=gnu++2b)