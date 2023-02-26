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

    void addChild(OverlayContainer *c) ;

    void layout() override ;

    void draw() override ;

private:

    std::vector<std::unique_ptr<OverlayContainer>> children_ ;
    Direction dir_ = DirectionRow ;
    JustifyContent justify_ = JustifyFlexStart ;
    AlignContent align_ = AlignFlexStart ;
    AlignItems align_items_ = AlignItemsStretch ;
    Wrap wrap_ = WrapNoWrap ;
};



}
