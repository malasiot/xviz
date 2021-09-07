#ifndef CLSIM_SCENE_KEYFRAME_HPP
#define CLSIM_SCENE_KEYFRAME_HPP

namespace clsim {

// a key frame is a value at a specific time. The time is in normalized coordinates [0, 1]

template<class T>
class KeyFrame {
public:
    KeyFrame(float t, T v): t_(t), value_(v) {}

    float timeStamp() const { return t_ ; }
    const T &value() const { return value_ ; }

    float t_ ;
    T value_ ;
};

} // namespace clsim


#endif
