#pragma once
#include <xviz/overlay/overlay.hpp>

class YGNode ;

namespace xviz {

class FlexBox: public OverlayContainer {
public:

    enum Direction { DirectionRow, DirectionColumn, DirectionRowReverse, DirectionColumnReverse } ;
    enum JustifyContent { JustifyFlexStart, JustifyFlexEnd, JustifyCenter, JustifySpaceBetween, JustifySpaceAround, JustifySpaceEvenly } ;
    enum AlignContent { AlignFlexStart, AlignFlexEnd, AlignCenter, AlignSpaceBetween, AlignSpaceAround, AlignSpaceEvenly } ;
    enum Wrap { WrapNoWrap, WrapWrap, WrapReverse } ;
    enum AlignItems { AlignItemsFlexStart, AlignItemsFlexEnd, AlignItemsCenter, AlignItemsStretch, AlignItemsBaseline } ;

    FlexBox() = default ;
    ~FlexBox() ;

    void addChild(OverlayContainer *c, float grow = 0, float shrink = 1) ;

    void layout() override ;

    void draw() override ;

    void measure(float &mw, float &mh) ;

    void setDirection(Direction dir) { dir_ = dir ; }

private:

    struct Child {
        std::unique_ptr<OverlayContainer> container_ ;
        float grow_, shrink_ ;
    };

    std::vector<Child> children_ ;
    Direction dir_ = DirectionRow ;
    JustifyContent justify_ = JustifyFlexStart ;
    AlignContent align_ = AlignFlexStart ;
    AlignItems align_items_ = AlignItemsStretch ;
    Wrap wrap_ = WrapNoWrap ;

    YGNode *makeTree() ;
};



}
