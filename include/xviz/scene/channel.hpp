#ifndef XVIZ_SCENE_CHANNEL_HPP
#define XVIZ_SCENE_CHANNEL_HPP

#include <xviz/scene/timeline.hpp>
#include <xviz/scene/easing.hpp>
#include <xviz/scene/keyframe_sampler.hpp>

namespace xviz {

// An abstract channel holds the value to be animated and updates its value its time the update function is called
// The time value t should be between 0 and 1 where 1 corresponds to animation cycle duration

class AbstractChannel {
public:
    virtual void update(float t) = 0 ;
};

// a standard channel over a specific type that intepolates key frames

template <class S> class TimeLineChannel: public AbstractChannel {
public:

    TimeLineChannel() {
        static LinearKeyFrameSampler<S> default_sampler ;
        static EaseLinear default_easing ;
        sampler_ = &default_sampler ;
        easing_curve_ = &default_easing ;
    }

    void setSampler(const KeyFrameSampler<S> *sampler) {
        sampler_ = sampler ;
    }

    void setEasing(const EasingCurve *ec) {
        easing_curve_ = ec ;
    }

    TimeLineChannel(const TimeLine<S> &f, const KeyFrameSampler<S> *sampler, const EasingCurve *ec):
    data_(f), sampler_(sampler), easing_curve_(ec) {}

    virtual void update(float ts) override {
        float tt = easing_curve_->value(ts) ;
        value_ = sampler_->interpolate(data_, tt) ;
    }

    TimeLine<S> &timeLine() { return data_ ; }
    const S &value() const { return value_ ; }

protected:

    S value_ ;
    TimeLine<S> data_ ;
    const KeyFrameSampler<S> *sampler_ ;
    const EasingCurve *easing_curve_ ;
};


} // namespace xviz


#endif
