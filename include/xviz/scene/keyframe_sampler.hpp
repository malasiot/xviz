#ifndef XVIZ_SCENE_KEYFRAME_SAMPLER_HPP
#define XVIZ_SCENE_KEYFRAME_SAMLPER_HPP

#include <xviz/scene/timeline.hpp>
#include <vector>
#include <Eigen/Geometry>

namespace xviz {

// concept of interpolation function between key frames

template <class T> class KeyFrameInterpolator {
    public:
    static T interpolate(float t, const T &kf1, const T &kf2) ;
};

template <class T> class LinearKeyFrameInterpolator {
    public:
    static T interpolate(float t, const T &kf1, const T &kf2) {
        return t * kf2 + ( 1.0f - t ) * kf1 ;
    }
};

template <class S> class LinearKeyFrameInterpolator<Eigen::Quaternion<S>> {
public:
    static Eigen::Quaternion<S> interpolate(float t, const Eigen::Quaternion<S> &kf1, const Eigen::Quaternion<S> &kf2) {
        return kf1.slerp(t, kf2) ;
    }
};

template <typename T>
class KeyFrameSampler {
public:
    virtual T interpolate(const TimeLine<T> &data, float fraction) const = 0 ;
};

template <typename T, template<class S> class Interpolator = LinearKeyFrameInterpolator>
class LinearKeyFrameSampler: public KeyFrameSampler<T> {
public:

    virtual T interpolate(const TimeLine<T> &data, float fraction) const override {

        int numKeyFrames = data.numKeyFrames() ;


        const auto &firstKeyFrame = data.getKeyFrame(0) ;
        const auto &lastKeyFrame = data.getKeyFrame(numKeyFrames-1) ;

        if ( numKeyFrames == 1)
            return firstKeyFrame.value() ;

        if ( fraction <= firstKeyFrame.timeStamp() ) { // extrapolate using the first two key frames
            const auto &nextKeyFrame = data.getKeyFrame(1) ;

            float intervalFraction = ( fraction - firstKeyFrame.timeStamp() ) /
                      ( nextKeyFrame.timeStamp() - firstKeyFrame.timeStamp() );
            return Interpolator<T>::interpolate(intervalFraction, firstKeyFrame.value(),  nextKeyFrame.value());
        } else if ( fraction >= lastKeyFrame.timeStamp() ) {
            const auto &prevKeyFrame = data.getKeyFrame(numKeyFrames - 2);

            float intervalFraction = ( fraction - prevKeyFrame.timeStamp() ) /
                      ( lastKeyFrame.timeStamp() - prevKeyFrame.timeStamp());
            return Interpolator<T>::interpolate(intervalFraction, prevKeyFrame.value(),
                          lastKeyFrame.value());
        }

        KeyFrame<T> prevKeyFrame = firstKeyFrame ;

        for (int i = 1; i < numKeyFrames; ++i ) {
            const auto &nextKeyFrame = data.getKeyFrame(i);
            if ( fraction < nextKeyFrame.timeStamp() ) {
                 float intervalFraction = (fraction - prevKeyFrame.timeStamp()) /
                          ( nextKeyFrame.timeStamp() - prevKeyFrame.timeStamp()) ;
                 return Interpolator<T>::interpolate(intervalFraction, prevKeyFrame.value(),
                              nextKeyFrame.value());
            }
            prevKeyFrame = nextKeyFrame;
        }

        // shouldn't reach here
        return lastKeyFrame.value();

    }

};

} // namespace xviz


#endif
