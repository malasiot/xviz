set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets WebSockets Charts REQUIRED)

set( QT_CLIENT_SOURCES
    client.cpp
    client.hpp
    session_config.cpp
    session_config.hpp
    chart_panel.cpp
    chart_panel.hpp
    panel_config.hpp
    panel_config.cpp
    qt_graphics_helpers.cpp
    qt_graphics_helpers.hpp

    chart/chart_widget.hpp
    chart/chart_widget.cpp
    chart/line_chart.hpp
    chart/line_chart.cpp
    chart/bar_chart.hpp
    chart/bar_chart.cpp
    chart/axis.cpp
    chart/axis.hpp
    chart/tics.cpp
    chart/tics.hpp
    chart/legend.hpp
    chart/legend.cpp

    panel.cpp
    panel.hpp
    image_panel.cpp
    image_panel.hpp
    image_widget.cpp
    image_widget.hpp
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

target_link_libraries(xviz_qt_client xviz Qt5::Core Qt5::Widgets Qt5::WebSockets Qt5::Charts ${PROTOBUF_LIBRARY})

install(TARGETS xviz_qt_client DESTINATION bin)
