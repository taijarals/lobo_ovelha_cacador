cmake_minimum_required(VERSION 3.20)

project(grupo_1 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.c)
add_executable(grupo_1 ${SRC})

option(MO_GUI "Desativa a compilação do modo gráfico" OFF)

if(MO_GUI)
    target_compile_definitions(grupo_1 PRIVATE MO_GUI)
endif()


find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED raylib)


target_include_directories(grupo_1 PRIVATE
        include
        ${RAYLIB_INCLUDE_DIRS}
)

target_link_libraries(grupo_1 PRIVATE
        ${RAYLIB_LIBRARIES}
        m
)

target_compile_options(grupo_1 PRIVATE
        -Wall
        -Wextra
)

# Copia os assets do jogo pra o diretório de build
add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_SOURCE_DIR}/bin/assets
)
add_dependencies(grupo_1 copy_assets)
