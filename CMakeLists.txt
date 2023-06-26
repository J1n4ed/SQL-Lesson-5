cmake_minimum_required(VERSION 3.26)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP ON)
set(PostgreSQL_ADDITIONAL_VERSIONS "15")

project(DbaseWork VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("${PROJECT_SOURCE_DIR}\\..\\..\\libpqxx-7.7.5" libpqxx-build)

add_executable(DbaseWork)
target_sources(DbaseWork
    PUBLIC
    main.cpp
)
target_sources(DbaseWork
  PUBLIC
    FILE_SET all_my_modules TYPE CXX_MODULES FILES
    "${PROJECT_SOURCE_DIR}/src/dbase.ixx"
)

target_compile_features(DbaseWork PRIVATE cxx_std_17)
target_link_libraries(DbaseWork pqxx)