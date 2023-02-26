#include <xviz/gui/viewer.hpp>

#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/light.hpp>
#include <xviz/gui/offscreen.hpp>
#include <xviz/scene/node_helpers.hpp>
#include <xviz/overlay/text.hpp>
#include <xviz/overlay/image_box.hpp>
#include <xviz/overlay/flex_box.hpp>
#include <xviz/overlay/canvas.hpp>

#include <random>
#include <iostream>

#include <QMainWindow>
#include <QApplication>
#include <QPainter>
#include <QDirIterator>
#include <QTimer>
#include <QDebug>

#include "util.hpp"

using namespace xviz ;
using namespace Eigen ;
using namespace std ;

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 g_rng(rd()); //Standard mersenne_twister_engine seeded with rd()

static float rnd_uniform(float a, float b) {
   std::uniform_real_distribution<> dis(a, b);
   return dis(g_rng) ;
}


Matrix3f makeSkewSymmetric(const Vector3f& v) {
    Matrix3f result = Matrix3f::Zero();

    result(0, 1) = -v(2);
    result(1, 0) =  v(2);
    result(0, 2) =  v(1);
    result(2, 0) = -v(1);
    result(1, 2) = -v(0);
    result(2, 1) =  v(0);

    return result;
}

#define EPSILON_EXPMAP_THETA 1.0e-3

Eigen::Matrix3f expMapRot(const Vector3f& q) {
    float theta = q.norm();

    Matrix3f R = Matrix3f::Zero();
    Matrix3f qss =  makeSkewSymmetric(q);
    Matrix3f qss2 =  qss*qss;

    if (theta < EPSILON_EXPMAP_THETA)
        R = Matrix3f::Identity() + qss + 0.5*qss2;
    else
        R = Eigen::Matrix3f::Identity()
                + (sin(theta)/theta)*qss
                + ((1-cos(theta))/(theta*theta))*qss2;

    return R;
}

Isometry3f getRandTransform(double d)
{
    Isometry3f t = Isometry3f::Identity();

    Vector3f rotation(rnd_uniform(-M_PI, M_PI),  rnd_uniform(-M_PI, M_PI), rnd_uniform(-M_PI, M_PI)) ;
    Vector3f position(rnd_uniform(-0.8, 0.8), rnd_uniform(d, d + 0.1), rnd_uniform(-0.8, 0.8));

    t.translation() = position;
    t.linear() = expMapRot(rotation);

    return t;
}

NodePtr randomBox(ScenePtr &scene, const string &name, const Vector3f &hs, const Vector4f &clr) {

    NodePtr box_node(new Node) ;
    box_node->setName(name) ;

    GeometryPtr geom(new BoxGeometry(hs)) ;

    ConstantMaterial *material = new ConstantMaterial(clr) ;

    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;

    box_node->setTransform(getRandTransform(0)) ;

    scene->addChild(box_node) ;

    return box_node ;
}

NodePtr randomCylinder(ScenePtr &scene, const string &name, float r, float h, const Vector4f &clr) {

    NodePtr box_node(new Node) ;
    box_node->setName(name) ;

    GeometryPtr geom(new CylinderGeometry(r, h)) ;

    PhongMaterial *material = new PhongMaterial(clr) ;

    MaterialPtr mat(material) ;
    box_node->addDrawable(geom, mat) ;

    box_node->setTransform(getRandTransform(0)) ;

    scene->addChild(box_node) ;

    return box_node ;
}

class PickingViewer: public SceneViewer {
public:
    PickingViewer(ScenePtr sc, QWidget *parent = nullptr): SceneViewer(sc, parent)
    {
        setMouseTracking(true);
        setDefaultCamera() ;

        trackball_.setZoomScale(1.2) ;
        ray_caster_.setPointDistanceThreshold(0.05);
        ray_caster_.setLineDistanceThreshold(0.05);
   //    ray_caster_.buildOctrees();
        highlight_.reset(new ConstantMaterial({1, 0, 0, 1})) ;

      //  ray_caster_.addNode(scene_, true) ;

     OrthographicCamera *pcam = new OrthographicCamera(-1, 1, 1, -1, -1, 1) ;

     dec_camera_.reset(pcam) ;

     NodePtr box_node(new Node()) ;
     box_node->setTransform(Affine3f::Identity());

     GeometryPtr geom(new BoxGeometry({0.05,0.05, 0.05})) ;

     ConstantMaterial *material = new ConstantMaterial({0, 0.5, 0, 1}) ;

     MaterialPtr mat(material) ;
     box_node->addDrawable(geom, mat) ;
     dec_scene_ = box_node ;
    }

    void initializeGL() override {
        SceneViewer::initializeGL() ;

        TextBox *text = new TextBox() ;

        text->addTextSpan(new Text("Graphics", Font("Times", 32), {1, 0, 0})) ;
        text->newLine() ;
        counter_ = new Text("", Font("Times", 32), {0, 1, 0}) ;
        text->addTextSpan(counter_) ;
        text->setAlignment(TextBox::AlignBottom | TextBox::AlignLeft) ;
        text->setMargins(10, 10, 10, 10) ;

   //     text->setBackgroundColor({0.7, 0.7, 0.7}) ;
   //     text->setBorderWidth(4);
   //     text->setOpacity(0.5) ;
      //  text->layout() ;
     //   overlay_.addChild(text) ;

        ImageBox *image = new ImageBox(Image("/tmp/image.jpg")) ;

             image->setMargins(20, 10, 20, 10);
        image->setAlignment(ImageBox::AlignLeft);
   //     image->setBackgroundColor({0.9, 0.9, 0.9}) ;
    //    image->setBorderWidth(4);
    //    image->setOpacity(0.5) ;
      //  image->layout() ;

        Frame *frame1 = new Frame(text) ;
       // frame->setPosition(0, 0) ;
    //    frame1->setSize(200, 200) ;
     //   frame1->setMinHeight(100) ;
      //    frame1->setMaxHeight(200) ;
          frame1->setMaxWidth(600) ;

        frame1->setBackgroundColor({0.9, 0.9, 0.9}) ;
        frame1->setBorderWidth(1);
        frame1->setMargins(5, 0, 5, 0) ;


        Frame *frame2 = new Frame(image) ;
       // frame->setPosition(0, 0) ;
      //  frame2->setMinHeight(100);
     //   frame2->setMaxHeight(200) ;
        frame2->setMinWidth(200) ;

        frame2->setBackgroundColor({0.0, 0.9, 0.9}) ;
        frame2->setBorderWidth(1);

        box1_ = new FlexBox() ;
        box1_->addChild(frame1) ;
        box1_->addChild(frame2, 1) ;

        box2_ = new FlexBox() ;
        box2_->setDirection(FlexBox::DirectionColumn);

        Frame *frame3 = new Frame() ;
        frame3->setMinHeight(100) ;

        box2_->addChild(box1_) ;
        box2_->addChild(frame3) ;


    }


    void mouseMoveEvent(QMouseEvent *event) override {
        int x = event->x() ;
        int y = event->y() ;

        Ray ray = camera_->getRay(x, y) ;

        RayCastResult result ;
        if ( ray_caster_.intersectOne(ray, scene_->getNodesRecursive(), result) ) {
            if ( result.drawable_->geometry()->ptype() == Geometry::Triangles ) {
            cout << result.node_->name() << ' '
                 << result.triangle_idx_[0] << ' ' <<
                    result.triangle_idx_[1] << ' ' << result.triangle_idx_[2] << endl ;
            }
            else if ( result.drawable_->geometry()->ptype() == Geometry::Lines ) {
                cout << result.line_idx_[0] << ' ' <<
                        result.line_idx_[1] << endl ;
            }

            if ( result.drawable_ != selected_ ) {
                if ( old_ ) selected_->setMaterial(old_) ;
                selected_ = result.drawable_ ;
                old_ = selected_->material() ;
                selected_->setMaterial(highlight_) ;
                update() ;
            }
        } else {
            if ( selected_ ) {
                selected_->setMaterial(old_) ;
                selected_ = nullptr ;
                old_ = nullptr ;
                update() ;
            }
        }

        SceneViewer::mouseMoveEvent(event) ;
    }

    void mousePressEvent(QMouseEvent *event) override {
        PerspectiveCamera *pcam = dynamic_cast<PerspectiveCamera *>(camera_.get()) ;
        int x = event->x() ;
        int y = event->y() ;
        Vector3f p(0.5, 0.21, 2.0) ;
        auto u = pcam->project(p) ;
        auto v = pcam->unProject(u.x(), u.y(), u.z()) ;
        std::cout << v << std::endl ;

        if (event->button() == Qt::RightButton ) {
            RayCaster rc ;
            rc.setBackFaceCulling(false) ;

            int x = event->x() ;
            int y = event->y() ;

            Ray ray = camera_->getRay(x, y) ;
            vector<RayCastResult> results ;
            if ( rc.intersect(ray, scene_->getNodesRecursive(), results) ) {

            cout << "multiple hits ++" << endl ;
            for ( const auto &res: results ) {
                 cout << res.node_->name() << "(" << res.t_ << ")" << endl ;
            }
            cout << "multiple hits --" << endl ;
            }

        }
        SceneViewer::mousePressEvent(event) ;
    }

    void resizeGL(int w, int h) override {
        SceneViewer::resizeGL(w, h) ;
        dec_camera_->setViewport(w, h) ;
        dec_camera_->setAspectRatio(w/static_cast<float>(h));

        glViewport(0, 0, w, h) ;
        box2_->setSize(50.0_perc, 100.0_perc) ;
        box2_->layout() ;
    }

    void paintGL() override {

        static int counter = 0 ;
        SceneViewer::paintGL() ;

        decorator_.render(dec_scene_, dec_camera_, false) ;

        //rdr_.renderText("Graphics & Ideas", 20, 50, Font("Times", 32), {1, 0, 0});
        overlay_.draw() ;

        stringstream strm ;
        strm << "Forward " ;
        strm << counter ++ ;
        counter_->updateText(strm.str()) ;

        box2_->draw() ;

        Canvas canvas ;
        canvas.save() ;
        canvas.beginPath() ;
        canvas.moveTo(10, 10) ;
        canvas.lineTo(500, 500) ;
        canvas.stroke() ;

        canvas.beginPath();
        canvas.roundedRect(10, 10, 100, 50, 4);
        canvas.setPaint(0.2, 0.3, 0.5, 0.5) ;
        canvas.fill();

        canvas.setStrokeColor(0.4, 0.5, 0.2);
        canvas.setStrokeWidth(4) ;
        canvas.stroke() ;


#if 0
        for( unsigned int i=0 ; i<10 ; i++ ) {
            stringstream strm ;
            strm << "box" << i ;
            NodePtr node = scene_->findNodeByName(strm.str());
            if ( node ) {
                Vector3f c = node->geomCenter() ;
                Vector2f p = rdr_.project(c) ;
                // Render text
                QPainter painter(this);
                QString text = QString::fromStdString(strm.str()) ;
                QFontMetrics fm(painter.font()) ;
                QRect rect = fm.boundingRect(text) ;
                painter.drawText(p.x() - rect.width()/2, p.y() + rect.height()/2, text);
                painter.end();
            }

        }
#endif
    }

private:
    RayCaster ray_caster_ ;
    xviz::MaterialPtr highlight_, old_;
    Drawable *selected_ = nullptr ;
    Renderer decorator_ ;
    NodePtr dec_scene_ ;
    CameraPtr dec_camera_ ;

    OverlayGroup overlay_ ;
    FlexBox *box1_, *box2_ ;

    Text *counter_ ;

};

int main(int argc, char **argv)
{
    TestApplication app("picking", argc, argv) ;


    NodePtr model(new Node) ;
    model->load(TestApplication::data() + "/models/2CylinderEngine.glb", 0);
    Affine3f tr(Affine3f::Identity());
    tr.scale(0.001f) ;
    tr.translate(-model->geomCenter()) ;
    model->setTransform(tr) ;
    model->setName("model") ;

    ScenePtr scene(new Scene) ;

    scene->addChild(model) ;

    for( unsigned int i=0 ; i<10 ; i++ ) {
        Vector4f clr(0.5, rnd_uniform(0.0, 1.0), rnd_uniform(0.0, 1.0), 1.0) ;
        stringstream strm ;


        if ( i != 0 ) {
            strm << "box" << i ;
            string name = strm.str() ;
            NodePtr res = randomBox(scene, name, Vector3f(0.04, rnd_uniform(0.1, 0.15), 0.04), clr);

        }
        else {
            strm << "cylinder" << i ;
            randomCylinder(scene, strm.str(), rnd_uniform(0.05, 0.1), rnd_uniform(0.1, 0.15), clr);
        }
    }

    GeometryPtr ptcloud(new Geometry(Geometry::Points)) ;
    ptcloud->vertices().push_back({0, 0.1, 0}) ;
    ptcloud->vertices().push_back({0.1, 0.1, -0.1}) ;

    ptcloud->colors().push_back({1.0, 0.0, 0.0}) ;
    ptcloud->colors().push_back({0.0, 0.0, 1.0}) ;

    NodePtr cloudNode(new Node) ;
    cloudNode->addDrawable(ptcloud, MaterialPtr(new PerVertexColorMaterial())) ;

    scene->addChild(cloudNode) ;

    GeometryPtr lines(new Geometry(Geometry::Lines)) ;
    lines->vertices().push_back({0, 0.1, 0}) ;
    lines->vertices().push_back({0.1, 0.1, -0.1}) ;
    lines->vertices().push_back({0.2, -0.1, -0.3}) ;
    lines->vertices().push_back({0.2, 0.4, -0.1}) ;

    lines->indices() = {0, 1, 1, 2, 2, 3};


    NodePtr lineNode(new Node) ;
    lineNode->addDrawable(lines, MaterialPtr(new ConstantMaterial(Vector4f{0, 1, 0, 1}))) ;

    scene->addChild(lineNode) ;


    DirectionalLight *dl = new DirectionalLight(Vector3f(0.5, 0.5, 1)) ;
    dl->setDiffuseColor(Vector3f(1, 1, 1)) ;
    scene->addLightNode(LightPtr(dl)) ;


    SceneViewer::initDefaultGLContext();
    QMainWindow window ;
    window.setCentralWidget(new PickingViewer(scene)) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
}
