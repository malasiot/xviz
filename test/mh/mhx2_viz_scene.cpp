#include "mhx2_viz_scene.hpp"

#include <xviz/scene/geometry.hpp>
#include <xviz/scene/mesh.hpp>
#include <xviz/scene/material.hpp>

#include <numeric>

using namespace xviz ;
using namespace std ;
using namespace Eigen ;

static void computeTransformsRecursive(NodePtr n, Affine3f pmat)
{
    Node *parent = n->parent() ;
    Affine3f ioffset = n->transform() ;
    if ( parent == nullptr )
        n->transform() = ioffset ;
    else
        n->transform() =  pmat.inverse() * ioffset  ;

    for( NodePtr child: n->children())
    {
        computeTransformsRecursive(child, ioffset) ;
    }

}

MHNode::MHNode(const MHX2Model &model) {
    createGeometry(model) ;
}

void MHNode::createGeometry(const MHX2Model &model)
{


    // make skeleton

    std::map<string, int> bone_map ;
    vector<Geometry::Bone> bones ;
    vector<int> root_bones ;

    for( const auto &kv: model.bones_ ) {
        const MHX2Bone &bone = kv.second ;
        bone_map[kv.first] = bones.size()  ;

        Geometry::Bone b ;
        b.name_ = kv.first ;
        b.offset_ = bone.bmat_.inverse() ;
        b.node_ = make_shared<Node>() ;
        b.node_->setName(b.name_) ;
        b.node_->transform() = bone.bmat_ ;

        bones.push_back(b) ;
    }

    // build hierarchy

    for( const auto &kv: model.bones_ ) {
        const MHX2Bone &bone = kv.second ;
        string name = kv.first ;

        int idx = bone_map[name] ;

        if ( bone.parent_.empty() ) root_bones.push_back(idx) ;
        else {
            int pidx = bone_map[bone.parent_] ;
            bones[pidx].node_->addChild(bones[idx].node_) ;
        }
    }

    // compute relative bone transforms

    for( int i: root_bones ) {
        computeTransformsRecursive(bones[i].node_, Affine3f::Identity()) ;
        addChild(bones[i].node_) ;
    }

    // iterate over all meshes

    for( const auto &geom_kv: model.geometries_) {
        const MHX2Geometry &geom = geom_kv.second ;
        const MHX2Mesh &mesh = geom.mesh_ ;

        std::shared_ptr<Geometry> m(new Geometry(Geometry::Triangles)) ;

        vector<int> indices ;
        vector<Vector2f> tcoords ;
        vector<int> face_vertices ;
        vector<Vector2f> face_tcoords ;

        for( const Vector3f &v: mesh.vertices_ ) {
            m->vertices().push_back(v + geom.offset_) ;
        }

        // add faces

        for(size_t i=0 ; i<mesh.faces_.size() ; i++)
        {
            const MHX2Face &f = mesh.faces_[i] ;


            for(size_t k=0 ; k<f.num_vertices_ ; k++)
            {
                uint idx = f.indices_[k] ;

                indices.push_back(idx) ;
                tcoords.push_back(f.tex_coords_[k]);
            }

            indices.push_back(-1) ;
        }

        // bone weights

        size_t n = m->vertices().size() ;

        m->weights().resize(n) ;

        m->skeleton().insert(m->skeleton().end(), bones.begin(), bones.end()) ;

        // collect weights per vertex, we need then to normalize and keep only max 4 weights

        vector<vector<int>> vg_idxs(n) ;
        vector<vector<float>> vg_weights(n) ;

        for ( const auto &kv: mesh.groups_ ) {
            const string &name = kv.first ;
            const MHX2VertexGroup &group = kv.second ;

            int bidx ;
            auto it = bone_map.find(name) ;
            if ( it == bone_map.end() ) continue ;
            else bidx = it->second ;

            if ( bidx != -1 )  {

                for( int i=0 ; i<group.idxs_.size() ; i++ )  {
                    int idx = group.idxs_.at(i) ;

                    if ( idx < 0 ) continue ;

                    vg_idxs[idx].push_back(bidx) ;
                    vg_weights[idx].push_back(group.weights_[i]) ;

                }
            }
        }

        for( int i=0 ; i<n ; i++ ) {
            auto &idxs = vg_idxs[i] ;
            auto &weights = vg_weights[i] ;

            auto &dest = m->weights()[i] ;

            // trim if neccessery
            if ( weights.size() > Geometry::MAX_BONES_PER_VERTEX ) {
                // sort

                // initialize original index locations
                  vector<size_t> sort_idx(weights.size());
                  std::iota(sort_idx.begin(), sort_idx.end(), 0);

                  // sort indexes based on comparing values in v
                  std::sort(sort_idx.begin(), sort_idx.end(),
                       [&weights](size_t i1, size_t i2) {
                           return weights[i1] > weights[i2];});

                  vector<int> trimmed_idxs ;
                  vector<float> trimmed_weights ;
                  for( size_t k=0 ; k<Geometry::MAX_BONES_PER_VERTEX ; k++ ) {
                      trimmed_idxs.push_back(idxs[sort_idx[k]]) ;
                      trimmed_weights.push_back(weights[sort_idx[k]]) ;
                  }

                  idxs = trimmed_idxs ;
                  weights = trimmed_weights ;
            }

            // normalize

            float w = 0.0 ;
            for ( float weight: weights ) w += weight ;
            for ( float &weight: weights ) weight /= w ;

            // copy to destination

            for( size_t k=0 ; k<idxs.size() ; k++ ) {
                dest.bone_[k] = idxs[k] ;
                dest.weight_[k] = weights[k] ;
            }

        }


        // split faces into triangles
        int c = 0 ;

        for( size_t i=0 ; i<indices.size() ; i++ )
        {
            int idx = indices[i] ;

            if ( idx == -1 )
            {
                for(size_t j=1 ; j<face_vertices.size() - 1 ; j++)
                {
                    m->indices().push_back(face_vertices[0]) ;
                    m->indices().push_back(face_vertices[j+1]) ;
                    m->indices().push_back(face_vertices[j]) ;

                    m->texCoords(0).push_back(face_tcoords[0]) ;
                    m->texCoords(0).push_back(face_tcoords[j+1]) ;
                    m->texCoords(0).push_back(face_tcoords[j]) ;
                }

                face_vertices.clear() ;
                face_tcoords.clear() ;

            }
            else {
                face_vertices.push_back(idx) ;
                face_tcoords.push_back(tcoords[c++]) ;
            }
        }

        m->computeNormals();

        auto *material = new PhongMaterial() ;

        auto matit = model.materials_.find(geom.material_) ;
        if ( matit == model.materials_.end() ) {
            material->setDiffuseColor({0.5, 0.5, 0.5}) ;
        } else {
            const MHX2Material &mat = matit->second ;
            material->setAmbientColor(Vector3f(mat.ambient_color_.x(), mat.ambient_color_.y(), mat.ambient_color_.z()));
            material->setDiffuseColor(Vector3f(mat.diffuse_color_.x(), mat.diffuse_color_.y(), mat.diffuse_color_.z()));
            material->setSpecularColor(Vector3f(mat.specular_color_.x(), mat.specular_color_.y(), mat.specular_color_.z()));
            material->setOpacity(mat.opacity_) ;
            material->setShininess(mat.shininess_) ;

        }

        addDrawable(m, std::shared_ptr<Material>(material));
    }
}

