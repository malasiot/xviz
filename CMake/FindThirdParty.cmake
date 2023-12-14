find_package(Eigen3 REQUIRED)
find_package(assimp REQUIRED)

if( POLICY CMP0072 )
  cmake_policy(SET CMP0072 NEW)
endif()

find_package(OpenGL QUIET REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)
find_package(PNG)

FIND_PACKAGE(Freetype REQUIRED)
FIND_PACKAGE(Fontconfig REQUIRED)
FIND_PACKAGE(HarfBuzz REQUIRED)

find_package(glfw3 REQUIRED)
