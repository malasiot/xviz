#ifndef CLSIM_PHYSICS_WORLD_HPP
#define CLSIM_PHYSICS_WORLD_HPP

#include <vector>
#include <memory>

namespace clsim {

class Cloth ;
class CollisionObject ;

class World {

public:

    std::unique_ptr<Cloth> cloth_ ;
    std::vector<std::unique_ptr<CollisionObject>> objects_ ;

};

}

#endif
