set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets WebSockets Xml REQUIRED)

set( QT_CLIENT_SOURCES
    ws_client.cpp
    ${XVIZ_INCLUDE_DIR}/qt/ws_client.hpp

    ui_element_factory.hpp
    ui_element_factory.cpp

    qt_graphics_helpers.cpp
    qt_graphics_helpers.hpp

    ${XVIZ_INCLUDE_DIR}/qt/annotation.hpp
    annotation.cpp

    resource_loader.cpp
    ${XVIZ_INCLUDE_DIR}/qt/resource_loader.hpp

    chart_element.cpp
    chart_element.hpp

    chart/chart_widget.cpp
    chart/line_chart.cpp
    chart/bar_chart.cpp
    chart/raster_chart.cpp
    chart/axis.cpp
    chart/tics.cpp
    chart/legend.cpp
    chart/color_ramp.cpp

    ${XVIZ_INCLUDE_DIR}/qt/chart/chart_widget.hpp
    ${XVIZ_INCLUDE_DIR}/qt/chart/line_chart.hpp
    ${XVIZ_INCLUDE_DIR}/qt/chart/bar_chart.hpp
    ${XVIZ_INCLUDE_DIR}/qt/chart/raster_chart.hpp
    ${XVIZ_INCLUDE_DIR}/qt/chart/axis.hpp
    ${XVIZ_INCLUDE_DIR}/qt/chart/tics.hpp
    ${XVIZ_INCLUDE_DIR}/qt/chart/legend.hpp
    ${XVIZ_INCLUDE_DIR}/qt/chart/color_ramp.hpp

    ${XVIZ_INCLUDE_DIR}/qt/scene/viewer.hpp
    ${XVIZ_INCLUDE_DIR}/qt/scene/trackball.hpp
    ${XVIZ_INCLUDE_DIR}/qt/scene/renderer.hpp
    ${XVIZ_INCLUDE_DIR}/qt/scene/mesh_data.hpp
    ${XVIZ_INCLUDE_DIR}/qt/scene/material.hpp
    ${XVIZ_INCLUDE_DIR}/qt/scene/offscreen.hpp

    scene/viewer.cpp
    scene/trackball.cpp
    scene/renderer.cpp
    scene/mesh_data.cpp
    scene/material.cpp
    scene/offscreen.cpp
    scene/shadow_map.cpp
#    scene/shadow_map_renderer.cpp
    scene/util.cpp

    image_element.cpp
    image_element.hpp

    image_widget.cpp

    image_tools.cpp
    image_rb.cpp
    image_rb.hpp
    ${XVIZ_INCLUDE_DIR}/qt/image_widget.hpp
    ${XVIZ_INCLUDE_DIR}/qt/image_tools.hpp

    tabular_element.cpp
    tabular_element.hpp

    scene_element.cpp
    scene_element.hpp

    ${XVIZ_INCLUDE_DIR}/qt/main_window.hpp
    main_window.cpp

)

set( QT_CLIENT_HEADERS
    client.hpp
)


#include_directories(${PROTOBUF_INCLUDE_DIR})
#protobuf_generate_cpp(PROTO_SRC PROTO_HDR ../../messages/session.proto)
#set_source_files_properties(${PROTO_SRC} ${PROTO_HDR} PROPERTIES GENERATED TRUE)


add_library(xviz_qt_client ${LIBRARY_TYPE} ${QT_CLIENT_SOURCES} ${QT_CLIENT_MOC_SOURCES} ${PROTO_SRC} ${PROTO_HDR})

target_link_libraries(xviz_qt_client xviz Qt5::Core Qt5::Widgets Qt5::WebSockets Qt5::Xml ${PROTOBUF_LIBRARY})


# Install library
install(TARGETS xviz_qt_client
  EXPORT ${PROJECT_EXPORT}
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT stlib
  COMPONENT dev)
