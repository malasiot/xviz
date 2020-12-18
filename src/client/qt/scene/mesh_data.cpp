#include <xviz/qt/scene/mesh_data.hpp>

#define POSITION_LOCATION    0
#define NORMALS_LOCATION    1
#define COLORS_LOCATION    2
#define BONE_ID_LOCATION    3
#define BONE_WEIGHT_LOCATION    4
#define UV_LOCATION 5

#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

MeshData::MeshData(const xviz::Mesh &mesh)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // Create the VAO

    vao_ = new QOpenGLVertexArrayObject;

    if ( vao_->create() ) vao_->bind();

    const xviz::Mesh::vb3_t &vertices = mesh.vertices() ;
    const xviz::Mesh::vb3_t &normals = mesh.normals() ;
    const xviz::Mesh::vb3_t &colors = mesh.colors() ;
    const xviz::Mesh::indices_t &indices = mesh.indices() ;

    elem_count_ = vertices.size() ;
    indices_ = indices.size() ;

    pos_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    pos_->create();
    pos_->bind();
    pos_->allocate(&vertices[0], vertices.size() * sizeof(GLfloat) * 3) ;
    f->glEnableVertexAttribArray(POSITION_LOCATION);
    f->glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    pos_->release() ;

    if ( !normals.empty() ) {
        normals_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        normals_->create();
        normals_->bind();
        normals_->allocate(&normals[0], normals.size() * sizeof(GLfloat) * 3) ;
        f->glEnableVertexAttribArray(NORMALS_LOCATION);
        f->glVertexAttribPointer(NORMALS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        normals_->release() ;
    }

    if ( !colors.empty() ) {
        colors_ = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        colors_->create();
        colors_->bind();
        colors_->allocate(&colors[0], colors.size() * sizeof(GLfloat) * 3) ;
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
            f->glEnableVertexAttribArray(UV_LOCATION+t);
            f->glVertexAttribPointer(UV_LOCATION+t, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            tex_coords_[t]->release() ;
        }
    }


/*
    const std::vector<Mesh::BoneWeight> weights = mesh.weights() ;
    if ( !weights.empty() ) {
        glGenBuffers(1, &weights_);
        glBindBuffer(GL_ARRAY_BUFFER, weights_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(weights[0]) * weights.size(), weights.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(BONE_ID_LOCATION);
        glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(Mesh::BoneWeight), (const GLvoid*)0);

        glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
        glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::BoneWeight), (const GLvoid*)offsetof(Mesh::BoneWeight, weight_));
    }
*/
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
}


MeshData::~MeshData()
{
    if ( pos_ ) delete pos_ ;
    if ( colors_ ) delete colors_ ;
    if ( normals_ ) delete normals_ ;
    for( uint i=0 ; i<max_textures_ ; i++ ) {
        if ( tex_coords_[i] ) delete tex_coords_[i] ;
    }
    if ( index_ )  delete index_ ;
}

