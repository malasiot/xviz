#ifndef XVIZ_RENDERER_MESH_DATA_HPP
#define XVIZ_RENDERER_MESH_DATA_HPP

#include <xviz/scene/geometry.hpp>

#include "gl/gl3w.h"

namespace xviz { namespace impl {

class MeshData {
public:

    MeshData() ;
    MeshData(const Geometry &mesh) ;

    void update(Geometry &mesh) ;

    static const int max_textures_ = 4 ;

    GLuint pos_ = 0, normals_ = 0, colors_ = 0, weights_ = 0, tex_coords_[max_textures_] = {0}, tf_ = 0, index_ = 0;
    GLuint vao_ ;
    GLuint elem_count_, indices_  ;

    ~MeshData() ;

} ;


}}

#endif
