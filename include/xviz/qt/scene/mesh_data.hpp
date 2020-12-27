#ifndef XVIZ_QT_SCENE_MESH_DATA_HPP
#define XVIZ_QT_SCENE_MESH_DATA_HPP

#include <xviz/scene/geometry.hpp>

#include <QOpenGLExtraFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class MeshData {
public:

    MeshData() ;
    MeshData(const xviz::Geometry &mesh) ;
    void update(xviz::Geometry &geom);

    static const int max_textures_ = 4 ;

    QOpenGLVertexArrayObject *vao_ = nullptr;
    QOpenGLBuffer *pos_ = nullptr, *normals_ = nullptr, *colors_ = nullptr, *weights_ = nullptr,
        *tex_coords_[max_textures_] = {nullptr}, *index_ = nullptr ;
    //GLuint pos_ = 0, normals_ = 0, colors_ = 0, weights_ = 0, tex_coords_[max_textures_] = {0}, tf_ = 0, index_ = 0;
    GLuint elem_count_, indices_  ;

    ~MeshData() ;

} ;


#endif
