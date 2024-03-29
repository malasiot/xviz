set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include_directories(
    3rdparty/
    ${EIGEN3_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
    ${FONTCONFIG_INCLUDE_DIR}
    ${GLFW3_IMCLUDE_DIRS}
)

set(LIB_SOURCES
    common/gl/gl3w.c
    common/shader.cpp
    common/resource_manager.cpp

    renderer/material_program.cpp
    renderer/renderer.cpp
    renderer/shadow_map.cpp
    renderer/mesh_data.cpp
    renderer/texture_data.cpp

    overlay/text_item.cpp
    overlay/glyph_cache.cpp
    overlay/font_manager.cpp
    overlay/gl_rect.cpp
    overlay/gl_image.cpp

    overlay/text_layout_engine.cpp
    overlay/scrptrun.cpp
    overlay/text_engine.cpp

    overlay/overlay.cpp
    overlay/text.cpp
    overlay/image_box.cpp
    overlay/flex_box.cpp
    overlay/frame.cpp
    overlay/canvas.cpp

    scene/node.cpp
    scene/assimp_loader.cpp
    scene/camera.cpp
    scene/geometry.cpp
    scene/intersect.cpp
    scene/octree.cpp
    scene/raycaster.cpp
    scene/node_helpers.cpp


    robot/robot_scene.cpp
    robot/urdf_loader.cpp
    robot/urdf_robot.cpp

    common/image.cpp
    common/transform.cpp
    common/vector.cpp
    common/matrix.cpp

    3rdparty/pugi/pugixml.cpp

    gui/viewer.cpp
    gui/trackball.cpp
    gui/offscreen.cpp
    gui/manipulator.cpp

#    gui/viewer_glfw.cpp
#    gui/translation_manipulator.cpp
#    gui/rotation_manipulator.cpp

    ${LIBRARY_INCLUDE_DIR}/gui/viewer.hpp
    ${LIBRARY_INCLUDE_DIR}/gui/offscreen.hpp
)

file(GLOB YOGA_SOURCES CONFIGURE_DEPENDS
    3rdparty/yoga/*.cpp
    3rdparty/yoga/**/*.cpp)

list(APPEND LIB_SOURCES ${YOGA_SOURCES} 3rdparty/nanovg/nanovg.c)

set_property(SOURCE 3rdparty/nanovg/nanovg.c APPEND PROPERTY COMPILE_DEFINITIONS "NVG_NO_STB")

add_library(xviz SHARED ${LIB_SOURCES})


if ( PNG_FOUND )
    target_compile_definitions(xviz PUBLIC HAS_LIBPNG)
endif()

if (msvc)
target_link_libraries(xviz assimp::assimp ${OPENGL_LIBRARIES}  ${PNG_LIBRARIES} Qt5::Core Qt5::Widgets)
else ()
target_link_libraries(xviz  assimp ${OPENGL_LIBRARIES} ${PNG_LIBRARIES} ${FREETYPE_LIBRARIES} ${Fontconfig_LIBRARIES} glfw Qt5::Core Qt5::Widgets)
endif ()

# Install library
install(TARGETS xviz
  EXPORT ${PROJECT_EXPORT}
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT stlib
  COMPONENT dev)


# Install headers
install(DIRECTORY "${LIBRARY_INCLUDE_DIR}" DESTINATION "${INSTALL_INCLUDE_DIR}/xviz" )
