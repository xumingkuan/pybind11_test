cmake_minimum_required(VERSION 3.25)
project(pybind11_test)

set(CMAKE_CXX_STANDARD 17)

# python
if (MSVC)
    # Could not find the "Development" component on Windows. Using the following commands
    # to get Python_INCLUDE_DIRS and Python_LIBRARY_DIRS.
    find_package(Python COMPONENTS Interpreter)
    execute_process(COMMAND ${Python_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_path('include'))"
            OUTPUT_VARIABLE Python_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${Python_EXECUTABLE} -c "import distutils.command.build_ext;
d = distutils.core.Distribution();
b = distutils.command.build_ext.build_ext(d);
b.finalize_options();
print(*b.library_dirs, sep=';')"
            OUTPUT_VARIABLE Python_LIBRARY_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    find_package(Python COMPONENTS Interpreter Development)
endif()
message(STATUS "Python include directories: ${Python_INCLUDE_DIRS}")
include_directories(${Python_INCLUDE_DIRS})
message(STATUS "Python library directories: ${Python_LIBRARY_DIRS}")
link_directories(${Python_LIBRARY_DIRS})

# pybind11
execute_process(COMMAND ${Python_EXECUTABLE} -m pybind11 --cmake
        OUTPUT_VARIABLE pybind11_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
find_package(pybind11 CONFIG REQUIRED)
include_directories(${pybind11_INCLUDE_DIR})

add_executable(pybind11_test main.cpp)
target_link_libraries(pybind11_test PRIVATE pybind11::embed)
