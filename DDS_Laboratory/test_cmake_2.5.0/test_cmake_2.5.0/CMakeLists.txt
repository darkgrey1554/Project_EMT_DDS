# CMakeList.txt: проект CMake для test_cmake_2.5.0; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

if(NOT fastcdr_FOUND)
    find_package(fastcdr REQUIRED)
endif()

if(NOT fastrtps_FOUND)
    find_package(fastrtps REQUIRED)
endif()

include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(-std=c++11)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

#file(GLOB_RECURSE DDS_LIB "C:\\Program Files\\eProsima\\fastrtps 2.5.0\\lib\\x64Win64VS2019\\*.lib")
include_directories("C:\\Program Files\\eprosima\\fastrtps 2.5.0\\include")
#target_link_libraries(test_cmake_2.5.0  ${DDS_LIB})
# Добавьте источник в исполняемый файл этого проекта.

link_libraries(fastcdr)
link_libraries(fastrtps)

add_executable (test_cmake_2.5.0 "test_cmake_2.5.0.cpp" "test_cmake_2.5.0.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
