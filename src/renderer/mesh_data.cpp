#include "mesh_data.hpp"

#include <iostream>

#define POSITION_LOCATION    0
#define NORMALS_LOCATION    1
#define COLORS_LOCATION    2
#define BONE_ID_LOCATION    3
#define BONE_WEIGHT_LOCATION    4
#define UV_LOCATION 5

namespace xviz { namespace impl {

MeshData::MeshData(const Geometry &mesh) {

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    const Geometry::vb3_t &vertices = mesh.vertices() ;
    const Geometry::vb3_t &normals = mesh.normals() ;
    const Geometry::vb3_t &colors = mesh.colors() ;
    const Geometry::indices_t &indices = mesh.indices() ;

    elem_count_ = vertices.size() ;
    indices_ = indices.size() ;

    glGenBuffers(1, &pos_);
    glBindBuffer(GL_ARRAY_BUFFER, pos_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat) * 3, &vertices[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    if ( !normals.empty() ) {
        glGenBuffers(1, &normals_);
        glBindBuffer(GL_ARRAY_BUFFER, normals_);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat) * 3, (GLfloat *)normals.data(), GL_DYNAMIC_DRAW );
        glEnableVertexAttribArray(NORMALS_LOCATION);
        glVertexAttribPointer(NORMALS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    if ( !colors.empty() ) {
        glGenBuffers(1, &colors_);
        glBindBuffer(GL_ARRAY_BUFFER, colors_);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat) * 3, (GLfloat *)colors.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(COLORS_LOCATION);
        glVertexAttribPointer(COLORS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    for( int t = 0 ; t<mesh.numUVChannels() ; t++ ) {
        if ( !mesh.texCoords(t).empty() ) {
            glGenBuffers(1, &tex_coords_[t]);
            glBindBuffer(GL_ARRAY_BUFFER, tex_coords_[t]);
            glBufferData(GL_ARRAY_BUFFER, mesh.texCoords(t).size() * sizeof(GLfloat) * 2, (GLfloat *)mesh.texCoords(t).data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(UV_LOCATION + t);
            glVertexAttribPointer(UV_LOCATION + t, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        }
    }

    const auto &weights = mesh.weights() ;
    if ( !weights.empty() ) {
        glGenBuffers(1, &weights_);
        glBindBuffer(GL_ARRAY_BUFFER, weights_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(weights[0]) * weights.size(), weights.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(BONE_ID_LOCATION);
        glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(Geometry::BoneWeight), (const GLvoid*)0);

        glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
        glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(Geometry::BoneWeight), (const GLvoid*)offsetof(Geometry::BoneWeight, weight_));
    }

#if 0
    glGenBuffers(1, &tf_);
    glBindBuffer(GL_ARRAY_BUFFER, tf_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat) * 3, 0, GL_STATIC_READ);
#endif

    if ( !indices.empty() ) {
        glGenBuffers(1, &index_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void MeshData::release() {
    manager_->release(geom_) ;
}


void MeshData::update(Geometry &geom) {

    const Geometry::vb3_t &vertices = geom.vertices() ;
    const Geometry::vb3_t &normals = geom.normals() ;
    const Geometry::vb3_t &colors = geom.colors() ;

    if ( geom.verticesUpdated() ) {
        glBindBuffer(GL_ARRAY_BUFFER, pos_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GLfloat) * 3, &vertices[0]);
        glEnableVertexAttribArray(POSITION_LOCATION);
        glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        geom.setVerticesUpdated(false) ;
    }

    if ( !normals.empty() && geom.normalsUpdated()) {
        glBindBuffer(GL_ARRAY_BUFFER, normals_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(GLfloat) * 3, &normals[0]);
        glEnableVertexAttribArray(NORMALS_LOCATION);
        glVertexAttribPointer(NORMALS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        geom.setNormalsUpdated(false) ;
    }

    if ( !colors.empty() && geom.colorsUpdated() ) {
        glBindBuffer(GL_ARRAY_BUFFER, colors_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(GLfloat) * 3, &colors[0]);
        glEnableVertexAttribArray(NORMALS_LOCATION);
        glVertexAttribPointer(NORMALS_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        geom.setColorsUpdated(false) ;
    }


}

MeshData::~MeshData() {

    if ( pos_ ) glDeleteBuffers(1, &pos_) ;
    if ( normals_ ) glDeleteBuffers(1, &normals_) ;
    if ( colors_ ) glDeleteBuffers(1, &colors_) ;

    for( unsigned i=0 ; i<max_textures_ ; i++ ) {
        if ( tex_coords_[i] )
            glDeleteBuffers(1, &tex_coords_[i]);
    }

    if ( weights_ )
        glDeleteBuffers(1, &weights_);

    if ( index_ )
        glDeleteBuffers(1, &index_);

    glDeleteVertexArrays(1, &vao_) ;

}

MeshDataManager::~MeshDataManager() {
//    flush() ;
    dirty_ = true ;
    for( auto &p: meshes_ ) {
        Geometry *geom = p.first ;
        geom->data_ = nullptr ;
    }
}

MeshData *MeshDataManager::fetch(Geometry*geom) {
    MeshData *data = nullptr ;

    auto it = meshes_.find(geom) ;
    if ( it == meshes_.end() ) {
        data = new MeshData(*geom) ;
        data->manager_ = this ;
        data->geom_ = geom ;
        geom->data_ = data ;
        meshes_.emplace(geom, std::unique_ptr<MeshData>(data)) ;
    } else
        data = (*it).second.get() ;

    data->update(*geom) ;

    return data ;
}

void MeshDataManager::release(Geometry *geom) {
    if ( dirty_ ) return ;

    to_delete_.push_back(geom) ;
}

void MeshDataManager::flush() {
    for( Geometry *geom: to_delete_ ){
        auto it = meshes_.find(geom) ;
        if ( it != meshes_.end() ) {
            meshes_.erase(it) ;
            geom->data_ = nullptr ;
        }
    }
}

}
               }
