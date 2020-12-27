#ifndef XVIZ_SCENE_NODE_ANIMATION_HPP
#define XVIZ_SCENE_NODE_ANIMATION_HPP

#include <xviz/scene/animation.hpp>
#include <xviz/scene/node.hpp>

namespace xviz {

class NodeAnimation: public Animation {
public:

    NodeAnimation(const NodePtr &target): target_(target) {
        addChannel(&translation_) ;
        addChannel(&rotation_) ;
        addChannel(&scaling_) ;
    }

    void addTranslationKeyFrame(float t, const Eigen::Vector3f &tr) {
        translation_.timeLine().addKeyFrame(t, tr);
    }

    void addRotationKeyFrame(float t, const Eigen::Quaternionf &q) {
        rotation_.timeLine().addKeyFrame(t, q);
    }

    void addScalingKeyFrame(float t, const Eigen::Vector3f &s) {
        scaling_.timeLine().addKeyFrame(t, s);
    }

    void addKeyFrame(float t, const Eigen::Vector3f &tr, const Eigen::Quaternionf &q, const Eigen::Vector3f &s) {
        addTranslationKeyFrame(t, tr);
        addRotationKeyFrame(t, q) ;
        addScalingKeyFrame(t, s) ;
    }

    void update(float t) override {
        Animation::update(t) ;

        if ( isRunning() ) {
            Eigen::Affine3f mat = Eigen::Translation3f(translation_.value()) * rotation_.value() * Eigen::Scaling(scaling_.value()) ;
            target_->setTransform(mat) ;
        }
    }

    NodePtr target_ ;

    TimeLineChannel<Eigen::Vector3f> translation_ ;
    TimeLineChannel<Eigen::Vector3f> scaling_ ;
    TimeLineChannel<Eigen::Quaternionf> rotation_ ;
};

}

#endif
