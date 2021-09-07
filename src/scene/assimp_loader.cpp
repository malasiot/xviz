#include <clsim/scene/scene.hpp>
#include <clsim/scene/node.hpp>
#include <clsim/scene/drawable.hpp>
#include <clsim/scene/mesh.hpp>
#include <clsim/scene/light.hpp>
#include <clsim/scene/material.hpp>
#include <clsim/scene/geometry.hpp>
#include <clsim/scene/node_animation.hpp>

#include <optional>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <Eigen/Dense>
#include <iostream>
#include <unordered_map>
#include <set>

using namespace std ;
using namespace Eigen ;

static Vector4f color4_to_float4(const aiColor4D &c) {
    return Vector4f(c.r, c.g, c.b, c.a) ;
}

namespace clsim {

namespace internal {

class AssimpImporter {
public:
    AssimpImporter(Node &sc, int options): scene_(sc), options_(options) {}

    MaterialPtr importMaterial(const aiScene *sc, const struct aiMaterial *mtl, const string &model_path) ;

    Node &scene_ ;

    map<const aiMesh *, GeometryPtr> meshes_ ;
    map<const aiMaterial *, MaterialPtr> materials_ ;
    map<string, LightPtr> lights_ ;
    map<string, CameraPtr> cameras_ ;
    map<string, NodePtr> node_map_ ;
    int options_ ;

    bool importMaterials(const string &mpath, const aiScene *sc);
    bool importMeshes(const aiScene *sc);
    bool importLights(const aiScene *sc);
    bool importAnimations(const aiScene *sc) ;
    bool importNodes(Node *pnode, const aiScene *sc, const aiNode *nd);
    bool import(const aiScene *sc, const std::string &fname);
    bool findSkeletonHierarchies();

    void getMaterialTexture(const aiScene *sc, PhongMaterial *material, const struct aiMaterial *mtl) ;
};

static void getPhongMaterial(PhongMaterial *material, const struct aiMaterial *mtl) {

    aiColor4D diffuse, specular, ambient;
    float shininess, strength;
    unsigned int max;

    if ( AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
        material->setDiffuseColor(color4_to_float4(diffuse)) ;
    }

    if ( AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular) ) {
        material->setSpecularColor(color4_to_float4(specular)) ;
    }

    if ( AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient) ) {
        material->setAmbientColor(color4_to_float4(ambient) ) ;
    }

    max = 1;
    aiReturn ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);

    if ( ret1 == AI_SUCCESS ) {
        max = 1;
     //   aiReturn ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
     //   if(ret2 == AI_SUCCESS) shininess = shininess * strength ;
        material->setShininess(shininess);
    }
    else {
        material->setShininess(0.0);
        material->setSpecularColor({0, 0, 0, 1});
    }
}

static Sampler2D::TextureMapMode convertMapMode(aiTextureMapMode m) {
    switch ( m ) {
    case aiTextureMapMode_Clamp:
        return Sampler2D::CLAMP ;
    case aiTextureMapMode_Decal:
        return Sampler2D::DECAL ;
    case aiTextureMapMode_Wrap:
        return Sampler2D::WRAP ;
    }
}

static Image readTexture(aiTexture *t) {

    if ( t->mHeight == 0 ) {
        return Image(reinterpret_cast<u_char *>(t->pcData), ImageFormat::encoded, t->mWidth, 0);
    } else {
        uint sz = t->mWidth * t->mHeight;

        std::unique_ptr<unsigned char []> bytes(new unsigned char [sz * 4]) ;
        unsigned char *p = bytes.get() ;

        for ( uint j = 0; j < sz; ++j ) {
            *p++ = t->pcData[j].r;
            *p++ = t->pcData[j].g;
            *p++ = t->pcData[j].b;
            *p++ = t->pcData[j].a;
        }

        return Image(bytes.get(), ImageFormat::rgba32, t->mWidth, t->mHeight) ;
    }
}

void AssimpImporter::getMaterialTexture(const aiScene *sc, PhongMaterial *material, const struct aiMaterial *mtl) {
    aiString tex_path ;
    aiTextureMapping tmap ;
    aiTextureMapMode mode[3] ;
    ai_real blend ;

    if ( AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path, &tmap, 0, &blend, 0, mode) ) {

        if ( tex_path.data == nullptr || tex_path.length == 0 ) return ;

        Image image ;

        if ( *tex_path.data == '*' ) {
            int texture_id = atoi(tex_path.data + 1) ;
            aiTexture *texture = sc->mTextures[texture_id] ;

            image = readTexture(texture) ;
        } else {
            image = Image(string(tex_path.data, tex_path.length));
        }

        material->setDiffuseTexture(new Texture2D(image,
              Sampler2D(convertMapMode(mode[0]), convertMapMode(mode[1])))) ;
    }
}


MaterialPtr AssimpImporter::importMaterial(const aiScene *sc, const struct aiMaterial *mtl, const string &model_path) {

    int shading_model ;
    mtl->Get((const char *)AI_MATKEY_SHADING_MODEL, shading_model);

    PhongMaterial *material  = new PhongMaterial() ;

    getMaterialTexture(sc, material, mtl) ;
    getPhongMaterial(material, mtl) ;

    return MaterialPtr(material) ;

}

bool AssimpImporter::importMaterials(const string &mpath, const aiScene *sc) {
    for( uint m=0 ; m<sc->mNumMaterials ; m++ ) {
        const aiMaterial *material = sc->mMaterials[m] ;
        MaterialPtr smat = importMaterial(sc, material, mpath) ;
        materials_[material] = smat ;
    }

    return true ;
}

bool AssimpImporter::importMeshes(const aiScene *sc) {

    for( uint m=0 ; m<sc->mNumMeshes ; m++ ) {
        const aiMesh *mesh = sc->mMeshes[m] ;

        //   if ( mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE ) continue ;

        GeometryPtr smesh ;

        if ( mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE ) smesh.reset(new Geometry(Geometry::Triangles)) ;
        else if ( mesh->mPrimitiveTypes == aiPrimitiveType_LINE ) smesh.reset(new Geometry(Geometry::Lines)) ;
        else if ( mesh->mPrimitiveTypes == aiPrimitiveType_POINT ) smesh.reset(new Geometry(Geometry::Points)) ;
        else continue ;

        if ( mesh->HasPositions() ) {
            uint n = mesh->mNumVertices ;
            smesh->vertices().resize(n) ;

            for(int i = 0; i < n; ++i)
                smesh->vertices().data()[i] = Vector3f(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z) ;
        }

        if ( mesh->HasNormals() ) {
            uint n = mesh->mNumVertices ;
            smesh->normals().resize(n) ;

            for(int i = 0; i < n; ++i)
                smesh->normals().data()[i] = Vector3f(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) ;
        }

        if ( mesh->HasVertexColors(0) ) {
            uint n = mesh->mNumVertices ;
            smesh->colors().resize(n) ;

            for(int i = 0; i < n; ++i)
                smesh->colors().data()[i] = Vector3f(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b) ;
        }

        for( uint t=0 ; t<MAX_TEXTURES ; t++ ) {
            uint n = mesh->mNumVertices ;
            if ( mesh->HasTextureCoords(t) ) {
                smesh->texCoords(t).resize(n) ;

                for(int i = 0; i < n; ++i)
                    smesh->texCoords(t).data()[i] = Vector2f(mesh->mTextureCoords[t][i].x, mesh->mTextureCoords[t][i].y) ;
            }
        }

        if ( mesh->HasFaces() ) {
            uint n = mesh->mNumFaces ;
            smesh->indices().resize(n * 3) ;

            for(int i = 0, k=0; i < n ; i++) {
                smesh->indices()[k++] = mesh->mFaces[i].mIndices[0];
                smesh->indices()[k++] = mesh->mFaces[i].mIndices[1];
                smesh->indices()[k++] = mesh->mFaces[i].mIndices[2];
            }
        }

        if ( ( options_ & Scene::IMPORT_SKELETONS ) && mesh->HasBones() ) {
            std::map<string, int> bone_map ;

            int num_bones = mesh->mNumBones ;
            smesh->skeleton().resize(num_bones) ;

            smesh->weights().resize(mesh->mNumVertices) ;

            for( size_t i=0 ; i<num_bones ; i++ ) {
                aiBone *bone = mesh->mBones[i] ;
                string name = bone->mName.C_Str() ;

                Geometry::Bone &b = smesh->skeleton().at(i) ;
                b.name_.assign(std::move(name)) ;

                aiMatrix4x4 m = bone->mOffsetMatrix;

                b.offset_.matrix() << m.a1, m.a2, m.a3, m.a4,
                        m.b1, m.b2, m.b3, m.b4,
                        m.c1, m.c2, m.c3, m.c4,
                        m.d1, m.d2, m.d3, m.d4 ;

                // copy weights

                for( size_t j=0 ; j<bone->mNumWeights ; j++ ) {
                    aiVertexWeight &weight = bone->mWeights[j] ;
                    Geometry::BoneWeight &w = smesh->weights().at(weight.mVertexId) ;
                    size_t idx ;
                    for( idx = 0 ; w.bone_[idx] != -1 && idx < Geometry::MAX_BONES_PER_VERTEX ; ++idx ) ;
                    assert(idx < Geometry::MAX_BONES_PER_VERTEX) ;
                    w.bone_[idx] = i ;
                    w.weight_[idx] = weight.mWeight ;
                }

            }

        }

        meshes_[mesh] = smesh ;
    }

    return true ;
}

bool AssimpImporter::importLights(const aiScene *sc) {
    for( uint m=0 ; m<sc->mNumLights ; m++ ) {
        const aiLight *light = sc->mLights[m] ;

        LightPtr slight ;

        switch ( light->mType ) {
        case aiLightSource_DIRECTIONAL:
        {
            DirectionalLight *dl = new DirectionalLight({light->mDirection.x, light->mDirection.y, light->mDirection.z}) ;
            dl->diffuse_color_ << light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b ;
            dl->specular_color_ << light->mColorSpecular.r, light->mColorSpecular.g, light->mColorSpecular.b ;
            dl->ambient_color_ << light->mColorAmbient.r, light->mColorAmbient.g, light->mColorAmbient.b ;

            slight.reset(dl) ;
            break ;
        }
        case aiLightSource_POINT:
        {
            PointLight *pl = new PointLight({light->mPosition.x, light->mPosition.y, light->mPosition.z}) ;

            pl->diffuse_color_ << light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b ;
            pl->specular_color_ << light->mColorSpecular.r, light->mColorSpecular.g, light->mColorSpecular.b ;
            pl->ambient_color_ << light->mColorAmbient.r, light->mColorAmbient.g, light->mColorAmbient.b ;

            pl->constant_attenuation_ = light->mAttenuationConstant ;
            pl->linear_attenuation_ = light->mAttenuationLinear ;
            pl->quadratic_attenuation_ = light->mAttenuationQuadratic ;

            slight.reset(pl) ;
            break ;

        }
        case aiLightSource_SPOT:
        {
            SpotLight *sl = new SpotLight({light->mPosition.x, light->mPosition.y, light->mPosition.z},
                                    {light->mDirection.x, light->mDirection.y, light->mDirection.z}) ;

            sl->diffuse_color_ << light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b ;
            sl->specular_color_ << light->mColorSpecular.r, light->mColorSpecular.g, light->mColorSpecular.b ;
            sl->ambient_color_ << light->mColorAmbient.r, light->mColorAmbient.g, light->mColorAmbient.b ;

            sl->constant_attenuation_ = light->mAttenuationConstant ;
            sl->linear_attenuation_ = light->mAttenuationLinear ;
            sl->quadratic_attenuation_ = light->mAttenuationQuadratic ;
            sl->falloff_angle_ = light->mAngleOuterCone ;
            sl->falloff_exponent_ = 0 ;

            slight.reset(sl) ;
            break ;

        }

        }

        if ( slight ) {
            slight->name_ = light->mName.C_Str() ;
            lights_[slight->name_] = slight ;
        }
    }

    return true ;
}

/*
struct ChannelAffine {
    TimeLineChannel<Vector3f> translation_, scaling_ ;
    TimeLineChannel<Quaternionf> rotation_ ;
};

class NodeAnimation: public Animation {
public:

    void update(float t) override {
        Animation::update(t) ;

        if ( isRunning() ) {
            for( const auto &lp: node_map_ ) {
                const ChannelAffine &tr = *lp.first ;
                NodePtr node = lp.second ;

                Affine3f mat = Translation3f(tr.translation_.value()) * tr.rotation_.value() * Scaling(tr.scaling_.value()) ;

                node->setTransform(mat) ;
            }
        }
    }


    std::vector<ChannelAffine> channels_ ;
    std::map<ChannelAffine *, NodePtr> node_map_ ;
};
*/
bool AssimpImporter::importAnimations(const aiScene *sc)
{
    for( int i=0 ; i< sc->mNumAnimations ; i++ ) {
        aiAnimation *anim = sc->mAnimations[i] ;

        for( int j = 0 ; j<anim->mNumChannels ; j++ ) {
            aiNodeAnim *channel = anim->mChannels[j] ;

            string node_name(channel->mNodeName.C_Str()) ;
            auto it = node_map_.find(node_name) ;
            if ( it == node_map_.end() ) continue ;

            NodePtr node = it->second ;

            NodeAnimation *nanim = new NodeAnimation(node) ;
            nanim->setDuration(anim->mDuration * 1000.0/anim->mTicksPerSecond) ;

            if ( channel->mNumPositionKeys == 1 ) {
                const aiVectorKey &key = channel->mPositionKeys[0] ;
                nanim->addTranslationKeyFrame(0.0, Vector3f(key.mValue.x, key.mValue.y, key.mValue.z)) ;
                nanim->addTranslationKeyFrame(1.0, Vector3f(key.mValue.x, key.mValue.y, key.mValue.z)) ;
            } else {
                for( uint k=0 ; k<channel->mNumPositionKeys ; k++ ) {
                    const aiVectorKey &key = channel->mPositionKeys[k] ;
                    nanim->addTranslationKeyFrame(key.mTime/anim->mDuration, Vector3f(key.mValue.x, key.mValue.y, key.mValue.z)) ;
                }
            }
            if ( channel->mNumScalingKeys == 1 ) {
                const aiVectorKey &key = channel->mScalingKeys[0] ;
                nanim->addScalingKeyFrame(0.0, Vector3f(key.mValue.x, key.mValue.y, key.mValue.z)) ;
                nanim->addScalingKeyFrame(1.0, Vector3f(key.mValue.x, key.mValue.y, key.mValue.z)) ;
            } else {
                for( uint k=0 ; k<channel->mNumScalingKeys ; k++ ) {
                    const aiVectorKey &key = channel->mScalingKeys[k] ;
                    nanim->addScalingKeyFrame(key.mTime/anim->mDuration, Vector3f(key.mValue.x, key.mValue.y, key.mValue.z)) ;
                }
            }

            if ( channel->mNumRotationKeys == 1 ) {
                const aiQuatKey &key = channel->mRotationKeys[0] ;
                nanim->addRotationKeyFrame(0.0, Quaternionf(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)) ;
                nanim->addRotationKeyFrame(1.0, Quaternionf(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)) ;
            } else {
                for( uint k=0 ; k<channel->mNumScalingKeys ; k++ ) {
                    const aiQuatKey &key = channel->mRotationKeys[k] ;
                    nanim->addRotationKeyFrame(key.mTime/anim->mDuration, Quaternionf(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)) ;
                }
            }

            node->addAnimation(nanim) ;
        }

    }

    return true ;


}

bool AssimpImporter::importNodes(Node *pnode, const struct aiScene *sc, const struct aiNode* nd)
{
    unsigned int i;
    unsigned int n = 0, t;
    aiMatrix4x4 m = nd->mTransformation;
    /* update transform */

    NodePtr snode(new Node) ;

    Matrix4f tf ;
    tf << m.a1, m.a2, m.a3, m.a4,
            m.b1, m.b2, m.b3, m.b4,
            m.c1, m.c2, m.c3, m.c4,
            m.d1, m.d2, m.d3, m.d4 ;

    snode->setTransform(Affine3f(tf)) ;

    string nname(nd->mName.C_Str()) ;
    snode->setName(nname);

    node_map_.emplace(nname, snode) ;

    /* draw all meshes assigned to this node */
    for (; n < nd->mNumMeshes; ++n) {

        const aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

        map<const aiMesh *, GeometryPtr>::const_iterator mit = meshes_.find(mesh) ;

        if ( mit == meshes_.end() ) continue ;

        GeometryPtr geom = mit->second ;

        const aiMaterial* material = sc->mMaterials[mesh->mMaterialIndex];

        map<const aiMaterial *, MaterialPtr>::const_iterator cit = materials_.find(material) ;

        MaterialPtr mat ;

        if ( cit != materials_.end() )
            mat = cit->second ;

        snode->addDrawable(geom, mat) ;
    }

    auto lit = lights_.find(nname) ;
    if ( lit != lights_.end() )  // this is a light node
        snode->setLight(lit->second) ;

    auto camit = cameras_.find(nname) ;
    if ( camit != cameras_.end() )  ;// this is a camera node, no special handling

    if ( pnode ) pnode->addChild(snode) ;

    /* import all children */
    for (n = 0; n < nd->mNumChildren; ++n) {
        if ( !importNodes(snode.get(), sc, nd->mChildren[n]) )
            return false ;
    }

    if ( snode->parent() == nullptr ) {



        scene_.addChild(snode) ;
    }

    return true ;
}

// map bone names to nodes in the graph


bool AssimpImporter::findSkeletonHierarchies() {
    for( auto lp: meshes_ ) {
        GeometryPtr &geom = lp.second ;
        if ( geom->hasSkeleton() ) {
            auto &skeleton = geom->skeleton() ;
            std::set<Node *> nodes ;
            for ( Geometry::Bone &b: skeleton ) {
                auto it = node_map_.find(b.name_) ;
                if ( it == node_map_.end() ) return false ;
                NodePtr node = it->second ;
                b.node_ = node ;
                nodes.insert(node.get()) ;
            }
            // find node parent
            for( Node *n: nodes ) {
                Node *parent = n->parent() ;
                if ( parent == nullptr ) break ;
                if ( nodes.count(parent) == 0 ) {
                    geom->skeletonInverseGlobalTransform() = parent->globalTransform().inverse() ;
                    break ;
                }
            }
        }

    }

    return true ;
}

bool AssimpImporter::import(const aiScene *sc, const std::string &fname) {

    if ( !importMeshes(sc) ) return false ;
    if ( !importMaterials(fname, sc) ) return false ;

    if ( options_ & Node::IMPORT_LIGHTS ) {
        if ( !importLights(sc) ) return false ;
    }

    // This is  needed for Collada models that define an UP axis
    // in that case the root element transform is not identity
    sc->mRootNode->mTransformation = aiMatrix4x4() ;

    if ( !importNodes(nullptr, sc, sc->mRootNode) ) return false ;

    if ( options_ & Node::IMPORT_ANIMATIONS ) {
        if ( !importAnimations(sc) ) return false ;
    }

    if ( !findSkeletonHierarchies() ) return false ;

    return true ;
}

}

void Node::load(const std::string &fname, int options) {
  //  const aiScene *sc = aiImportFile(fname.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs | aiProcess_TransformUVCoords);
    const aiScene *sc = aiImportFile(fname.c_str(),
    aiProcess_GenNormals
  | aiProcess_Triangulate
  | aiProcess_FixInfacingNormals
  | aiProcess_JoinIdenticalVertices
  | aiProcess_SortByPType
  | aiProcess_OptimizeMeshes
  | aiProcess_LimitBoneWeights
  |  aiProcess_GenUVCoords
                                     ) ;
    if ( !sc ) {
        throw SceneLoaderException(aiGetErrorString(), fname) ;
    }

    load(sc, fname, options) ;

    aiReleaseImport(sc) ;
}

void Node::load(const aiScene *sc, const std::string &fname, int options) {

    internal::AssimpImporter importer(*this, options) ;

    bool res = importer.import(sc, fname) ;

    if ( !res ) {
        aiReleaseImport(sc) ;
        throw SceneLoaderException("Error while parsing assimp scene", fname) ;
    }

}



}

