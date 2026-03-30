cmake_minimum_required(VERSION 3.22)

set(SDL_X11_XSCRNSAVER OFF CACHE BOOL "" FORCE)
set(SDL_X11_TEST OFF CACHE BOOL "" FORCE)
set(SDL_X11_XTEST OFF CACHE BOOL "" FORCE)

project(Clayborne VERSION 0.1 LANGUAGES C CXX)

option(CLAYBORNE_VENDORED "Use vendored libraries" OFF)

if(CLAYBORNE_VENDORED)
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

add_executable(clayborne)

target_link_libraries(clayborne PRIVATE SDL3::SDL3)

target_sources(clayborne PRIVATE
    src/main.cpp
)

set_property(TARGET clayborne PROPERTY CXX_STANDARD 23)
set_property(TARGET clayborne PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET clayborne PROPERTY CXX_EXTENSIONS OFF)

if (MSVC)
    target_compile_options(clayborne PRIVATE
        /W4
        /WX
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
    )
else()
    target_compile_options(clayborne PRIVATE
        -Wall
        -Wextra
        -Werror
        -Wshadow
        -Wconversion
        -Wdouble-promotion
        -Wformat=2
        -Wundef
        -Wnull-dereference
        -Wpedantic
        -Wimplicit-fallthrough
        -Wmissing-declarations
        -Wcast-qual
        -Wold-style-cast
        -Woverloaded-virtual
        -Wnon-virtual-dtor
        -Wzero-as-null-pointer-constant
        $<$<CONFIG:Debug>:-O0 -g>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )
endif()