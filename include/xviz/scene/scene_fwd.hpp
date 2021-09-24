#ifndef XVIZ_SCENE_FWD_HPP
#define XVIZ_SCENE_FWD_HPP

#include <memory>

namespace xviz {

struct Material ;
typedef std::shared_ptr<Material> MaterialPtr ;

class Mesh ;
typedef std::shared_ptr<Mesh> MeshPtr ;

struct Camera ;
typedef std::shared_ptr<Camera> CameraPtr ;

struct Light ;
typedef std::shared_ptr<Light> LightPtr ;

class Scene ;
typedef std::shared_ptr<Scene> ScenePtr ;

class Node ;
typedef std::shared_ptr<Node> NodePtr ;
typedef std::shared_ptr<const Node> ConstNodePtr ;

class Drawable ;
typedef std::shared_ptr<Drawable> DrawablePtr ;

class Geometry ;
typedef std::shared_ptr<Geometry> GeometryPtr ;


class Camera ;
typedef std::shared_ptr<Camera> CameraPtr ;

}

#endif
