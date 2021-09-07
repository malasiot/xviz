#include "mesh_data.hpp"

#define POSITION_LOCATION    0
#define NORMALS_LOCATION    1
#define COLORS_LOCATION    2
#define BONE_ID_LOCATION    3
#define BONE_WEIGHT_LOCATION    4
#define UV_LOCATION 5

namespace clsim { namespace impl {


MeshData::MeshData(const Geometry &mesh)
{
   /*
    vao_ = new QOpenGLVertexArrayObject;

    if ( vao_->create() ) vao_->bind();

    const Geometry::vb3_t &vertices = mesh.vertices() ;
    const Geometry::vb3_t &normals = mesh.normals() ;
    const Geometry::vb3_t &colors = mesh.colors() ;
    const Geometry::indices_t &indices = mesh.indices() ;

    elem_count_ = vertices.size() ;
    indices_ = indices.size() ;

    pos_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    pos_->create();
    pos_->bind();
    pos_->allocate(&vertices[0], vertices.size() * sizeof(GLfloat) * 3) ;
    pos_->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    f->glEnableVertexAttribArray(POSITION_LOCATION);
    f->glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    pos_->release() ;

    if ( !normals.empty() ) {
        normals_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        normals_->create();
        normals_->bind();
        normals_->allocate(&normals[0], normals.size() * sizeof(GLfloat) * 3) ;
        normals_->setUsagePattern(QOpenGLBuffer::DynamicDraw);
        f->glEnableVertexAttribArray(NORMALS_LOCATION);
        f->glVertexAttribPointer(NORMALS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        normals_->release() ;
    }

    if ( !colors.empty() ) {
        colors_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        colors_->create();
        colors_->bind();
        colors_->allocate(&colors[0], colors.size() * sizeof(GLfloat) * 3) ;
        colors_->setUsagePattern(QOpenGLBuffer::DynamicDraw);
        f->glEnableVertexAttribArray(COLORS_LOCATION);
        f->glVertexAttribPointer(COLORS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        colors_->release() ;
    }

    for( uint t = 0 ; t<mesh.numUVChannels() ; t++ ) {
        if ( !mesh.texCoords(t).empty() ) {
            tex_coords_[t]  = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
            tex_coords_[t]->create();
            tex_coords_[t]->bind();
            tex_coords_[t]->allocate(&mesh.texCoords(t)[0], mesh.texCoords(t).size() * sizeof(GLfloat) * 2) ;
            tex_coords_[t]->setUsagePattern(QOpenGLBuffer::StaticDraw);
            f->glEnableVertexAttribArray(UV_LOCATION+t);
            f->glVertexAttribPointer(UV_LOCATION+t, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            tex_coords_[t]->release() ;
        }
    }

    const auto &weights = mesh.weights() ;
    if ( !weights.empty() ) {
        weights_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        weights_->create();
        weights_->bind();
        weights_->allocate(&weights[0], weights.size() * sizeof(Geometry::BoneWeight)) ;

        f->glEnableVertexAttribArray(BONE_ID_LOCATION);
        f->glVertexAttribIPointer(BONE_ID_LOCATION, Geometry::MAX_BONES_PER_VERTEX, GL_INT, sizeof(Geometry::BoneWeight), (const GLvoid*)0);

        f->glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
        f->glVertexAttribPointer(BONE_WEIGHT_LOCATION, Geometry::MAX_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(Geometry::BoneWeight), (const GLvoid*)offsetof(Geometry::BoneWeight, weight_));
    }

#if 0
    glGenBuffers(1, &tf_);
    glBindBuffer(GL_ARRAY_BUFFER, tf_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat) * 3, 0, GL_STATIC_READ);
#endif

    if ( !indices.empty() ) {
        index_ = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        index_->create();
        index_->bind();
        index_->allocate(&indices[0], indices.size() * sizeof(uint32_t)) ;
        index_->release() ;
    }

    vao_->release() ;

    */
}


void MeshData::update(Geometry &geom) {


    /*
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    const Geometry::vb3_t &vertices = geom.vertices() ;
    const Geometry::vb3_t &normals = geom.normals() ;
    const Geometry::vb3_t &colors = geom.colors() ;

    if ( geom.verticesUpdated() ) {
        pos_->bind();
        pos_->write(0, &vertices[0], vertices.size() * sizeof(GLfloat) * 3);
        f->glEnableVertexAttribArray(POSITION_LOCATION);
        f->glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        geom.setVerticesUpdated(false) ;
    }

    if ( !normals.empty() && geom.normalsUpdated() ) {
        normals_->bind();
        normals_->write(0, &normals[0], normals.size() * sizeof(GLfloat) * 3);
        f->glEnableVertexAttribArray(NORMALS_LOCATION);
        f->glVertexAttribPointer(NORMALS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        geom.setNormalsUpdated(false) ;
    }

    if ( !colors.empty() && geom.colorsUpdated()) {
        colors_->bind();
        colors_->write(0, &colors[0], colors.size() * sizeof(GLfloat) * 3);

        f->glEnableVertexAttribArray(COLORS_LOCATION);
        f->glVertexAttribPointer(COLORS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        geom.setColorsUpdated(false) ;
    }

    */

}

MeshData::~MeshData()
{
    /*
    if ( pos_ ) delete pos_ ;
    if ( colors_ ) delete colors_ ;
    if ( normals_ ) delete normals_ ;
    for( uint i=0 ; i<max_textures_ ; i++ ) {
        if ( tex_coords_[i] ) delete tex_coords_[i] ;
    }

    if ( weights_ ) delete weights_ ;
    if ( index_ )  delete index_ ;
    */
}

}
}
