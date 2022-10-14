cmake_minimum_required(VERSION 3.8)

project(pineapple-package-manager LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXE_NAME ppkg)

find_package(PkgConfig REQUIRED)

pkg_check_modules(libzip REQUIRED libzip IMPORTED_TARGET)

add_executable(${EXE_NAME}
    main.cpp
)

target_link_libraries(${EXE_NAME}
PRIVATE
    PkgConfig::libzip
)
