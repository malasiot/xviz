set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets WebSockets REQUIRED)

set( QT_CLIENT_SOURCES
    client.cpp
    client.hpp
    session_config.cpp
    session_config.hpp
    panel_config.hpp
    panel_config.cpp
    main.cpp
)

set( QT_CLIENT_HEADERS
    client.hpp
)

set(QT_CLIENT_FORMS )
#SET(QT_CLIENT_RESOURCES mdi.qrc )

add_executable(xviz_qt_client ${QT_CLIENT_SOURCES} ${QT_CLIENT_MOC_SOURCES} ${QT_CLIENT_RC_SOURCES})

target_link_libraries(xviz_qt_client Qt5::Core Qt5::Widgets Qt5::WebSockets)

install(TARGETS xviz_qt_client DESTINATION bin)
