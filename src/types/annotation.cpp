#include <xviz/drawable.hpp>

#include "drawable.pb.h"

namespace xviz {

Brush Drawable::brush() const {
   if ( brush_.type() != BrushType::NoBrush ) return brush_ ;
   if ( parent_ ) return parent_->brush() ;
   return Brush() ;
}

Pen Drawable::pen() const {
    if ( pen_.type() != NoPen ) return pen_ ;
    if ( parent_ ) return parent_->pen() ;
    return Pen() ;
}

Matrix2d Drawable::transform() const {
    return transform_ ;
}

Matrix2d Drawable::globalTransform() const {
    if ( parent_ ) return parent_->globalTransform().postmult(transform_) ;
    else return transform_ ;
}

msg::Drawable *Drawable::write(const Drawable *d) {
    msg::Drawable *msg = new msg::Drawable ;

    msg::Brush *b = Brush::write(d->brush()) ;
    msg->set_allocated_brush(b) ;

    msg::Pen *p = Pen::write(d->pen()) ;
    msg->set_allocated_pen(p) ;

    if ( const GroupDrawable *gd = dynamic_cast<const GroupDrawable *>(d) ) {
        for(  auto &c : gd->children() ) {
            msg::Drawable *child = Drawable::write(c.get()) ;
            msg->mutable_children()->AddAllocated(child);
        }
    } else if ( const ShapeDrawable *sd = dynamic_cast<const ShapeDrawable *>(d) ) {
        msg::ShapeDrawable *msg_shape = new msg::ShapeDrawable ;
        msg_shape->set_allocated_path(Path::write(sd->path())) ;
        msg->set_allocated_shape_drawable(msg_shape) ;
    } else if ( const TextDrawable *td = dynamic_cast<const TextDrawable *>(d) ) {
        msg::TextDrawable *msg_text = new msg::TextDrawable ;
        msg_text->set_text(td->text()) ;
        msg_text->set_x(td->rect().x()) ;
        msg_text->set_x(td->rect().y()) ;
        msg->set_allocated_text_drawable(msg_text) ;
    }

    return msg ;
}

Drawable *Drawable::read(const msg::Drawable &msg) {
    Drawable *d ;

    if ( msg.has_shape_drawable() ) {
        const msg::ShapeDrawable &sd = msg.shape_drawable() ;
        d = new ShapeDrawable(Path::read(sd.path()));
    } else if ( msg.has_group_drawable() ) {
        const msg::GroupDrawable &gd = msg.group_drawable() ;
        GroupDrawable *dr = new GroupDrawable ;
        for( const auto &child: msg.children() )
            dr->addChild(Drawable::read(child)) ;
        d = dr ;
    } else if ( msg.has_text_drawable() ) {
        const msg::TextDrawable &td = msg.text_drawable() ;
        /*
        TextDrawable *dr = new TextDrawable(td.tex()) ;
        for( const auto &child: msg.children() )
            dr->addChild(Drawable::read(child)) ;
            */
     //   d = td ;
    }

    if ( d == nullptr ) return nullptr ;

    if ( msg.has_brush() ) {
        d->setBrush(Brush::read(msg.brush()));
    }
    if ( msg.has_pen() ) {
        d->setPen(Pen::read(msg.pen()));
    }

    return d ;
}






}
