set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets WebSockets Xml REQUIRED)

set( QT_CLIENT_SOURCES
    client.cpp
    client.hpp
    ui_element_factory.hpp
    ui_element_factory.cpp

    qt_graphics_helpers.cpp
    qt_graphics_helpers.hpp

    annotation.hpp
    annotation.cpp

    image_loader.cpp
    image_loader.hpp

    chart_element.cpp
    chart_element.hpp

    chart/chart_widget.hpp
    chart/chart_widget.cpp
    chart/line_chart.hpp
    chart/line_chart.cpp
    chart/bar_chart.hpp
    chart/bar_chart.cpp
    chart/raster_chart.cpp
    chart/raster_chart.hpp
    chart/axis.cpp
    chart/axis.hpp
    chart/tics.cpp
    chart/tics.hpp
    chart/legend.hpp
    chart/legend.cpp
    chart/color_ramp.cpp
    chart/color_ramp.hpp

    scene/viewer.hpp
    scene/viewer.cpp
    scene/trackball.cpp
    scene/trackball.hpp
    scene/renderer.cpp
    scene/renderer.hpp
    scene/mesh_data.cpp
    scene/mesh_data.hpp
    scene/material.cpp
    scene/material.hpp

    image_element.cpp
    image_element.hpp
    image_widget.cpp
    image_widget.hpp

    tabular_element.cpp
    tabular_element.hpp

    scene_element.cpp
    scene_element.hpp

    main_window.hpp
    main_window.cpp
    main.cpp
)

set( QT_CLIENT_HEADERS
    client.hpp
)

set(QT_CLIENT_FORMS )
#SET(QT_CLIENT_RESOURCES mdi.qrc )


include_directories(${PROTOBUF_INCLUDE_DIR})
protobuf_generate_cpp(PROTO_SRC PROTO_HDR ../../messages/session.proto)
set_source_files_properties(${PROTO_SRC} ${PROTO_HDR} PROPERTIES GENERATED TRUE)


add_executable(xviz_qt_client ${QT_CLIENT_SOURCES} ${QT_CLIENT_MOC_SOURCES} ${QT_CLIENT_RC_SOURCES} ${PROTO_SRC} ${PROTO_HDR})

target_link_libraries(xviz_qt_client xviz Qt5::Core Qt5::Widgets Qt5::WebSockets Qt5::Xml ${PROTOBUF_LIBRARY})

install(TARGETS xviz_qt_client DESTINATION bin)
