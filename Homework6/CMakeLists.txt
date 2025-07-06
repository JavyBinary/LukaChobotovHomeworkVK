cmake_minimum_required(VERSION 3.20)

project(app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h" "src/*.hpp")

add_executable(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_library(project_options INTERFACE)

target_compile_options(project_options INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
)

target_compile_options(project_options INTERFACE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

target_link_options(project_options INTERFACE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE project_options)

find_program(CLANG_TIDY_EXE clang-tidy)
if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
endif()

find_program(CLANG_FORMAT_EXE clang-format)
if(CLANG_FORMAT_EXE)
    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${SOURCES} ${HEADERS}
    )

    add_custom_target(
        check-format
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror -style=file ${SOURCES} ${HEADERS}
    )
endif()
