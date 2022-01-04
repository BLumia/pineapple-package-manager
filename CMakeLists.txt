cmake_minimum_required(VERSION 3.5)

project(pineapple-package-manager LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (EXE_NAME ppkg)

find_package(PkgConfig REQUIRED)

pkg_check_modules(libzip REQUIRED libzip)

add_executable(${EXE_NAME}

    main.cpp
)

target_include_directories(${EXE_NAME}
PRIVATE
    ${libzip_INCLUDE_DIRS}
)
target_link_libraries(${EXE_NAME}
PRIVATE
    ${libzip_LINK_LIBRARIES}
)
