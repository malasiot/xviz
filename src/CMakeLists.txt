add_subdirectory(client/qt)

set(XVIZ_LIB_SOURCES
    util/json_reader.cpp
    util/json_reader.hpp
)

add_library(xviz ${XVIZ_LIB_SOURCES} )
target_link_libraries(xviz)
