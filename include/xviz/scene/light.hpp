#ifndef XVIZ_SCENE_LIGHT_HPP
#define XVIZ_SCENE_LIGHT_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <Eigen/Core>
#include <xviz/scene/camera.hpp>

namespace xviz {

// Abstract light

struct Light {
    Light() = default ;
    virtual ~Light() {}

    void setName(const std::string &name) { name_ = name ; }
    const std::string name() const { return name_ ; }

    void setDiffuseColor(const Eigen::Vector3f &clr) {
        diffuse_color_ = clr ;
    }

    const Eigen::Vector3f &diffuseColor() const { return diffuse_color_ ; }

    void setAmbientColor(const Eigen::Vector3f &clr) {
        ambient_color_ = clr ;
    }

    const Eigen::Vector3f &ambientColor() const { return ambient_color_ ; }

    void setSpecularColor(const Eigen::Vector3f &clr) {
        specular_color_ = clr ;
    }

    const Eigen::Vector3f &specularColor() const { return specular_color_ ; }

    bool castsShadows() const { return casts_shadows_ ; }
    void setCastsShadows(bool v) { casts_shadows_ = v ; }

    void setShadowBias(float v) { shadow_bias_ = v ; }
    float shadowBias() const { return shadow_bias_ ; }

protected:

    std::string name_ ;
    bool casts_shadows_ = false ;

    Eigen::Vector3f diffuse_color_, specular_color_, ambient_color_ ;

    float shadow_bias_ = 0.0 ;
};


struct PointLight: public Light {
    PointLight(const Eigen::Vector3f &pos):  position_(pos) {}

    void setPosition(const Eigen::Vector3f &p) { position_ = p ;  }
    const Eigen::Vector3f &position() const { return position_ ; }

    void setConstantAttenuation(float v) { constant_attenuation_ = v ;  }
    float constantAttenuation() const { return constant_attenuation_ ; }

    void setLinearAttenuation(float v) { linear_attenuation_ = v ;  }
    float linearAttenuation() const { return linear_attenuation_ ; }

    void setQuadraticAttenuation(float v) { quadratic_attenuation_ = v ;  }
    float quadraticAttenuation() const { return quadratic_attenuation_ ; }

protected:

    Eigen::Vector3f position_ ;

    float constant_attenuation_ = 1.0 ;
    float linear_attenuation_ = 0.0;
    float quadratic_attenuation_ = 0.0;
};

struct DirectionalLight: public Light {
    DirectionalLight(const Eigen::Vector3f &position, const Eigen::Vector3f &target = {0, 0, 0}):
        position_(position), target_(target), shadow_cam_(OrthographicCamera(-5, 5, 5, -5, 0.01, 10)) {}

    void setPosition(const Eigen::Vector3f &p) { position_ = p ;  }
    const Eigen::Vector3f &position() const { return position_ ; }

    void setTarget(const Eigen::Vector3f &p) { target_ = p ;  }
    const Eigen::Vector3f &target() const { return target_ ; }

    void setShadowCamera(const OrthographicCamera &cam) { shadow_cam_ = cam ; }
    const OrthographicCamera &shadowCamera() const { return shadow_cam_ ; }

protected:

    Eigen::Vector3f position_, target_ ; // to correclty casts shadows set this to an absolute position (in this case the light direction is the normalised value)

public:

    OrthographicCamera shadow_cam_ ;

};

struct SpotLight: public Light {

    SpotLight(const Eigen::Vector3f &pos, const Eigen::Vector3f &dir):
       position_(pos), direction_(dir), shadow_cam_(PerspectiveCamera(1.0, M_PI/4)) {}

    void setPosition(const Eigen::Vector3f &p) { position_ = p ;  }
    const Eigen::Vector3f &position() const { return position_ ; }

    void setDirection(const Eigen::Vector3f &p) { direction_ = p ;  }
    const Eigen::Vector3f &direction() const { return direction_ ; }

    void setConstantAttenuation(float v) { constant_attenuation_ = v ;  }
    float constantAttenuation() const { return constant_attenuation_ ; }

    void setLinearAttenuation(float v) { linear_attenuation_ = v ;  }
    float linearAttenuation() const { return linear_attenuation_ ; }

    void setQuadraticAttenuation(float v) { quadratic_attenuation_ = v ;  }
    float quadraticAttenuation() const { return quadratic_attenuation_ ; }

    void setInnerCutoffAngle(float v) { inner_cutoff_angle_ = v ;  }
    float innerCutoffAngle() const { return inner_cutoff_angle_ ; }

    void setOuterCutoffAngle(float v) { outer_cutoff_angle_ = v ;  }
    float outerCutoffAngle() const { return outer_cutoff_angle_ ; }

    void setShadowCamera(const PerspectiveCamera &cam) { shadow_cam_ = cam ; }
    const PerspectiveCamera &shadowCamera() const { return shadow_cam_ ; }

protected:
    Eigen::Vector3f direction_, position_ ;

    float constant_attenuation_ = 1.0;
    float linear_attenuation_ = 0.0;
    float quadratic_attenuation_ = 0.0;
    float inner_cutoff_angle_ = M_PI, outer_cutoff_angle_ = M_PI ;

    PerspectiveCamera shadow_cam_ ;

};

using LightPtr = std::shared_ptr<Light> ;

} // namespace xviz
#endif
