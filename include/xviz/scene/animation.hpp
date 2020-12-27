#ifndef XVIZ_SCENE_ANIMATION_HPP
#define XVIZ_SCENE_ANIMATION_HPP

#include <memory>
#include <vector>
#include <limits>
#include <cmath>

#include <xviz/scene/channel.hpp>

namespace xviz {

class Animation {
public:
    enum RepeatMode {  RESTART, REVERSE  } ;

    Animation() {}

    // set/get the number of repetitions

    void setDuration(float value) {
       cycle_duration_ = value ;
    }

    float duration() const {
       return cycle_duration_ ;
    }

    // set/get the number of repetitions

    void setRepeatCount(int value) {
       repeat_count_ = value ;
    }

    int repeatCount() const {
       return repeat_count_ ;
    }

    // set/get the repetition mode when the animation reaches the end of the cycle
    // RESTART: start the animation from the beginning, REVERSE: start the animation from the target value towards the initial value

    void setRepeatMode(RepeatMode value) {
        repeat_mode_ = value ;
    }

    RepeatMode repeatMode() const {
        return repeat_mode_ ;
    }

    // add a new animation channel
    void addChannel(AbstractChannel *e) {
        channels_.emplace_back(e) ;
    }

    // update channels given the current time

    virtual void update(float t) {

        if ( is_running_ ) {
            float elapsed = t - start_time_ ;
            bool done = false ;

            float fraction = cycle_duration_ > 0 ? elapsed / cycle_duration_ : 1.f;
            if ( fraction >= 1.f ) {
                if ( cycle_ < repeat_count_ || repeat_count_ == -1 ) {

                    if ( repeat_mode_ == REVERSE )
                        reverse = !reverse ;

                    cycle_ += (int)fraction ;
                    fraction = fmod(fraction, 1.0f) ;

                    start_time_ += cycle_duration_ ;
                } else {
                    done = true ;
                    fraction = std::min(fraction, 1.0f);
                }
             }

            if (  reverse )
                fraction = 1.f - fraction;

            for( AbstractChannel *channel: channels_ ) {
                channel->update(fraction) ;
            }

            if ( done ) stop() ;
        }
    }

    // callbacks usefull for chaining animations

    virtual void onAnimationStarted() {}
    virtual void onAnimationStopped() {}

    // start the animation from current time stamp

    void start(float t) {
        if ( !is_running_ ) {
            start_time_ = t ;
        }
        is_running_ = true ;
        onAnimationStarted() ;
    }

    // stop animation

    void stop() {
        if ( is_running_  ) {
            is_running_ = false ;
        }
        onAnimationStopped() ;
    }

    bool isRunning() const { return is_running_ ; }

private:
    float cycle_duration_ = 0.f ;
    float start_time_ = 0.f ;
    bool reverse = false ;
    bool is_running_ = false ;
    int repeat_count_ = -1 ; // infinite
    RepeatMode repeat_mode_ = REVERSE ; // repeat mode
    int cycle_ = 0; // the current cycle ;
    std::vector<AbstractChannel *> channels_ ;
};

} // namespace xviz

#endif
