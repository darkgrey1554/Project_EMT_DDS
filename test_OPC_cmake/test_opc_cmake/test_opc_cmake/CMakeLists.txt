# CMakeList.txt: проект CMake для test_opc_cmake; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)



# Добавьте источник в исполняемый файл этого проекта.
add_executable (test_opc_cmake "test_opc_cmake.cpp" "test_opc_cmake.h")

include_directories("C:\\Program Files (x86)\\open62541\\include")

find_library(LIB_OPC open62541 "C:\\Program Files (x86)\\open62541\\lib")
message("--------------------------------------------")
message(lib path = ${LIB_OPC})
message("--------------------------------------------")

target_link_libraries(test_opc_cmake PUBLIC ${LIB_OPC})
target_link_libraries(test_opc_cmake PUBLIC ws2_32)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test_opc_cmake PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
