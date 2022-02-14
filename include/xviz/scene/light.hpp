#ifndef XVIZ_SCENE_LIGHT_HPP
#define XVIZ_SCENE_LIGHT_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <Eigen/Core>

namespace xviz {

// Abstract light

struct Light {
    Light() = default ;
    virtual ~Light() {}

    std::string name_ ;
    bool casts_shadows_ = false ;

    float shadow_cam_left_ = -5 ;
    float shadow_cam_right_ = 5 ;
    float shadow_cam_top_ = -5 ;
    float shadow_cam_bottom_ = 5 ;
    float shadow_cam_near_ = 0.01 ;
    float shadow_cam_far_ = 500.f ;
    float shadow_bias_ = 0.0 ;
};

struct AmbientLight: public Light {
    AmbientLight() {}
    AmbientLight(const Eigen::Vector3f &clr): color_(clr) {}

    Eigen::Vector3f color_ ;
};

struct PointLight: public Light {
    PointLight(const Eigen::Vector3f &pos):  position_(pos) {}

    Eigen::Vector3f position_ ;
    Eigen::Vector3f diffuse_color_, specular_color_, ambient_color_ ;
    float constant_attenuation_ = 1.0 ;
    float linear_attenuation_ = 0.0;
    float quadratic_attenuation_ = 0.0;
};

struct DirectionalLight: public Light {
    DirectionalLight(const Eigen::Vector3f &position, const Eigen::Vector3f &target = {0, 0, 0}):
        position_(position), target_(target) {}

    Eigen::Vector3f position_, target_ ; // to correclty casts shadows set this to an absolute position (in this case the light direction is the normalised value)
    Eigen::Vector3f diffuse_color_, specular_color_, ambient_color_ ;

};

struct SpotLight: public Light {

    SpotLight(const Eigen::Vector3f &pos, const Eigen::Vector3f &dir):
       position_(pos), direction_(dir) {}

    Eigen::Vector3f direction_, position_ ;
    Eigen::Vector3f diffuse_color_, specular_color_, ambient_color_ ;
    float constant_attenuation_ = 1.0;
    float linear_attenuation_ = 0.0;
    float quadratic_attenuation_ = 0.0;
    float inner_cutoff_angle_ = M_PI, outer_cutoff_angle_ = M_PI ;
};

using LightPtr = std::shared_ptr<Light> ;

} // namespace xviz
#endif
