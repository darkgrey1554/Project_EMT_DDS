# CMakeList.txt: проект CMake для CMakeProject1; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

if(NOT fastcdr_FOUND)
    find_package(fastcdr REQUIRED)
endif()

if(NOT fastrtps_FOUND)
    find_package(fastrtps REQUIRED)
endif()

# Добавьте источник в исполняемый файл этого проекта.
include_directories("C:/Program Files/eProsima/fastrtps 2.4.1/include")
link_directories("C:/Program Files/eProsima/fastrtps 2.4.1/lib/x64Win64VS2019")

add_definitions(-DFOONATHAN_MEMORY=1 -DFOONATHAN_MEMORY_VERSION_MAJOR=0 -DFOONATHAN_MEMORY_VERSION_MINOR=7 -DFOONATHAN_MEMORY_VERSION_PATCH=1 -DFASTRTPS_DYN_LINK -DFASTRTPS_DYN_LINK)

add_executable (CMakeroject1 "CMakeProject1.cpp" "CMakeProject1.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
