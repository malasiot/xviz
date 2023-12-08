#include <xviz/gui/viewer.hpp>
#include <xviz/gui/manipulator.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/light.hpp>

#include <random>
#include <iostream>

#include <QMainWindow>
#include <QApplication>
#include <QPainter>

#include "util.hpp"

using namespace xviz ;
using namespace Eigen ;
using namespace std ;

class Gui : public xviz::SceneViewer
{
public:
    Gui() ;
protected:

    void mousePressEvent(QMouseEvent *event) override {
        if ( gizmo_->onMousePressed(event) ) {
            update() ;
            return ;
        }

        SceneViewer::mousePressEvent(event) ;
    };

    void mouseReleaseEvent(QMouseEvent * event) override {
        if ( gizmo_->onMouseReleased(event) ) {
            update() ;
            return ;
        }

        SceneViewer::mouseReleaseEvent(event) ;
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if ( gizmo_->onMouseMoved(event) ) {
            update() ;
            return ;
        }

        SceneViewer::mouseMoveEvent(event) ;
    }

    void keyPressEvent(QKeyEvent *event) override {
        if ( event->key() == Qt::Key_L )
            gizmo_->setLocalTransform(true) ;
        else if ( event->key() == Qt::Key_G )
            gizmo_->setLocalTransform(false) ;
        else
            SceneViewer::keyPressEvent(event) ;
        update() ;
    }

private:
    std::shared_ptr<TransformManipulator> gizmo_ ;
};

Gui::Gui() {

    initCamera({0, 0, 0}, 4.0, UpAxis::ZAxis);

    NodePtr scene(new Node) ;

    NodePtr root_node(new Node) ;
    root_node->transform().translation() = Vector3f{1.0, 0.5f, 0} ;
    scene->addChild(root_node);

    NodePtr box_node(new Node) ;
    box_node->transform().translation() = Vector3f{0, 0.5f, 0} ;
    GeometryPtr geom(new BoxGeometry({0.5, 0.7, 0.4})) ;
    PhongMaterial *material = new PhongMaterial({1, 0, 1}, 1) ;
    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;
    root_node->addChild(box_node) ;

    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->setDiffuseColor(Vector3f(1, 1, 1)) ;
    scene->addLightNode(LightPtr(dl)) ;

    TransformManipulator *gizmo = new TransformManipulator(getCamera(), 2.0) ;
    gizmo->gizmo()->setOrder(2) ;
    gizmo->attachTo(box_node.get()) ;
    gizmo->setLocalTransform(false);
    gizmo_.reset(gizmo) ;



    setScene(scene) ;
    makeAxes(2.5) ;

}

int main(int argc, char **argv)
{
    TestApplication app("manipulators", argc, argv);
    SceneViewer::initDefaultGLContext();

    Gui *viewer = new Gui() ;

    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
