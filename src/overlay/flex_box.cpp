#include <xviz/overlay/flex_box.hpp>

#include "3rdparty/yoga/YGNode.h"

namespace xviz {
FlexBox::~FlexBox() {

}

void FlexBox::addChild(OverlayContainer *c, float grow, float shrink) {
    Child ce ;
    ce.container_.reset(c) ;
    ce.grow_ = grow ;
    ce.shrink_= shrink ;

    children_.emplace_back(std::move(ce)) ;
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

    float mw, mh, pw, ph ;
    c->measure(pw, ph) ;

    switch ( w_mode ) {
    case YGMeasureModeExactly:
        mw = w ;
        break ;
    case YGMeasureModeAtMost:
        mw = std::min(w, pw) ;
        break ;
    default:
        mw = pw ;
    }

    switch ( h_mode ) {
    case YGMeasureModeExactly:
        mh = h ;
        break ;
    case YGMeasureModeAtMost:
        mh = std::min(h, ph) ;
        break ;
    default:
        mh = ph ;
    }

    return { mw, mh };
}

void FlexBox::layout() {
    YGNode *root = makeTree() ;

    YGNodeStyleSetWidth(root, w_) ;
    YGNodeStyleSetHeight(root, h_) ;


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

YGNode *FlexBox::makeTree() {
    YGNode *root = YGNodeNew() ;

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
        OverlayContainer *container = c.container_.get() ;
        const YGNodeRef child = YGNodeNew();
        child->setContext(container);
        child->setMeasureFunc(_measure);

        if ( container->minHeight() )
            YGNodeStyleSetMinHeight(child, container->minHeight().value());
        if ( container->maxHeight() )
            YGNodeStyleSetMaxHeight(child, container->maxHeight().value());
        if ( container->minWidth() )
            YGNodeStyleSetMinWidth(child, container->minWidth().value());
        if ( container->maxWidth() )
            YGNodeStyleSetMaxWidth(child, container->maxWidth().value());

        YGNodeStyleSetFlexGrow(child, c.grow_);

        YGNodeStyleSetPositionType(child, YGPositionTypeRelative);
        YGNodeStyleSetFlexShrink(child, c.shrink_);
        YGNodeStyleSetFlexBasisAuto(child) ;
        YGNodeInsertChild(root, child, count++);
    }

    return root ;

}

void FlexBox::draw()
{
    for( const auto &c: children_ )
        c.container_->draw() ;
}

void FlexBox::measure(float &mw, float &mh)
{
    YGNode *root = makeTree() ;
    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
    mw = YGNodeLayoutGetWidth(root);
    mh = YGNodeLayoutGetHeight(root);

    YGNodeFreeRecursive(root);
}

}
