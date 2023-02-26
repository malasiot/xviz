#include <xviz/overlay/flex_box.hpp>

#include "3rdparty/yoga/YGNode.h"

namespace xviz {
FlexBox::~FlexBox() {

}

void FlexBox::addChild(OverlayContainer *c) {
    children_.emplace_back(c) ;
    c->setParent(this) ;
}

static MeasureMode yg_measure_mode(YGMeasureMode mode) {
    switch ( mode ) {
    case YGMeasureModeAtMost:
        return MeasureModeAtMost ;
    case YGMeasureModeExactly:
        return MeasureModeExact ;
    case YGMeasureModeUndefined:
        return MeasureModeUndefined ;
    }
}

static YGAlign yg_align_items(FlexBox::AlignItems mode) {
    switch ( mode ) {
    case FlexBox::AlignItemsFlexStart:
        return YGAlignFlexStart ;
    case FlexBox::AlignItemsFlexEnd:
        return YGAlignFlexEnd ;
    case FlexBox::AlignItemsCenter:
        return YGAlignCenter ;
    case FlexBox::AlignItemsStretch:
        return YGAlignStretch ;
    }
}

static YGJustify yg_justify(FlexBox::JustifyContent mode) {
    switch ( mode ) {
    case FlexBox::JustifyFlexStart:
        return YGJustifyFlexStart ;
    case FlexBox::JustifyFlexEnd:
        return YGJustifyFlexEnd ;
    case FlexBox::JustifyCenter:
        return YGJustifyCenter ;
    case FlexBox::JustifySpaceBetween:
        return YGJustifySpaceBetween ;
    case FlexBox::JustifySpaceAround:
        return YGJustifySpaceAround ;
    case FlexBox::JustifySpaceEvenly:
        return YGJustifySpaceEvenly ;

    }
}

static YGWrap yg_wrap(FlexBox::Wrap mode) {
    switch ( mode ) {
    case FlexBox::WrapNoWrap:
        return YGWrapNoWrap ;
    case FlexBox::WrapWrap:
        return YGWrapWrap ;
    case FlexBox::WrapReverse:
        return YGWrapWrapReverse ;
    }
}

static YGSize _measure(YGNode*node, float w, YGMeasureMode w_mode, float h, YGMeasureMode h_mode) {
    OverlayContainer *c = static_cast<OverlayContainer *>(node->getContext()) ;

    auto sz = c->measure(w, yg_measure_mode(w_mode), h, yg_measure_mode(h_mode)) ;
    return { sz.first, sz.second };
}

void FlexBox::layout() {
    YGNode *root = YGNodeNew() ;

    YGNodeStyleSetWidth(root, w_) ;
    YGNodeStyleSetHeight(root, h_) ;

    switch ( dir_ ) {
    case DirectionRow:
        YGNodeStyleSetFlexDirection(root, YGFlexDirectionRow) ; break ;
    case DirectionRowReverse:
        YGNodeStyleSetFlexDirection(root, YGFlexDirectionRowReverse) ; break ;
    case DirectionColumn:
        YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn) ; break ;
    case DirectionColumnReverse:
        YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumnReverse) ; break ;
   }

    YGNodeStyleSetAlignItems(root, yg_align_items(align_items_));
    YGNodeStyleSetJustifyContent(root, yg_justify(justify_));
    YGNodeStyleSetFlexWrap(root, yg_wrap(wrap_));

    size_t count = 0 ;
    for( const auto &c: children_ ) {
        const YGNodeRef child = YGNodeNew();
        child->setContext(c.get());
        child->setMeasureFunc(_measure);

        if ( c->minHeight() )
            YGNodeStyleSetMinHeight(child, c->minHeight().value());
        if ( c->maxHeight() )
            YGNodeStyleSetMaxHeight(child, c->maxHeight().value());
        if ( c->minWidth() )
            YGNodeStyleSetMinWidth(child, c->minWidth().value());
        if ( c->maxWidth() )
            YGNodeStyleSetMaxWidth(child, c->maxWidth().value());

        YGNodeStyleSetFlexGrow(child, c->stretch());

        YGNodeStyleSetPositionType(child, YGPositionTypeRelative);
        YGNodeStyleSetFlexShrink(child, 1);
        YGNodeStyleSetFlexBasisAuto(child) ;
        YGNodeInsertChild(root, child, count++);
    }


    YGNodeCalculateLayout(root, w_, h_, YGDirectionLTR);

    for( const auto &n: root->getChildren() ) {
        float left = YGNodeLayoutGetLeft(n);
        float top = YGNodeLayoutGetTop(n);
        float width = YGNodeLayoutGetWidth(n);
        float height = YGNodeLayoutGetHeight(n);

        OverlayContainer *c = static_cast<OverlayContainer *>(n->getContext()) ;

        c->setPosition(left, top) ;
        c->setSize(width, height) ;
        c->layout();

    }

    YGNodeFreeRecursive(root);
}

void FlexBox::draw()
{
    for( const auto &c: children_ )
        c->draw() ;
}

}
