#include <xviz/gui/viewer.hpp>
#include <xviz/gui/offscreen.hpp>
#include <xviz/scene/scene.hpp>
#include <xviz/scene/geometry.hpp>
#include <xviz/scene/light.hpp>

#include <xviz/scene/node_animation.hpp>

#include <random>
#include <iostream>

#include <QMainWindow>
#include <QApplication>

#include "util.hpp"



using namespace xviz ;
using namespace Eigen ;
using namespace std ;

static void replace_texture(NodePtr node, const string &tpath) {
    node->visit([&](Node &n) {
        cout << n.name() << endl ;

        if ( n.name() == "human_cmu-baseObject" ) {

            MaterialPtr mat = n.drawables()[0].material() ;

            ConstantMaterial *cmat = new ConstantMaterial({0, 0, 0, 1}) ;

            ImagePtr image(new Image(tpath)) ;
            Sampler2D sampler(Sampler2D::WRAP_CLAMP, Sampler2D::WRAP_CLAMP) ;
            sampler.setMagnification(Sampler2D::MAG_NEAREST) ;
            sampler.setMinification(Sampler2D::MIN_NEAREST_MIPMAP_NEAREST) ;

            Texture2D *texture = new Texture2D(image, Sampler2D()) ;
            cmat->setTexture(texture) ;
            n.drawables()[0].setMaterial(MaterialPtr(cmat)) ;

        }
    });
}

void set_geom_visibility(NodePtr node, bool visible) {
    node->visit([&](Node &n) {
        if ( n.name() != "human_cmu-baseObject" ) {
            if ( !n.drawables().empty() ) {
                cout << n.name() << endl ;
                n.setVisible(visible, true) ;
            }
        }
    });
}

static void hsv2rgb(float h, QRgb &rgb)
{
    int i ;
    float f, p, q, t, r, g, b ;

    if ( h == 0.0 ) return ;

    // h = 360.0-h ;

    h /= 60.0 ;

    i = (int)h ;
    f = h - i ;
    p = 0  ;
    q = 1-f ;
    t = f ;

    switch (i)
    {
    case 0:
        r = 1 ;
        g = t ;
        b = p ;
        break ;
    case 1:
        r = q ;
        g = 1 ;
        b = p ;
        break ;
    case 2:
        r = p ;
        g = 1 ;
        b = t ;
        break ;
    case 3:
        r = p ;
        g = q ;
        b = 1 ;
        break ;
    case 4:
        r = t ;
        g = p ;
        b = 1 ;
        break ;
    case 5:
        r = 1 ;
        g = p ;
        b = q ;
        break ;
    }

    rgb = qRgb((int)(255.0*r), (int)(255.0*g), (int)(255.0*b)) ;
}

const int nColors = 2 << 12 ;
QRgb *hsvlut ;

QImage imageToQImage(const Image &img) {
    int w = img.width(), h = img.height(), lw = w * 2 ;

    int nc = nColors ;

    if ( !hsvlut )
    {
        int c ;
        float h, hmax, hstep ;

        hsvlut = new QRgb [nColors] ;

        hmax = 180 ;
        hstep = hmax/nc ;

        for ( c=0, h=hstep ; c<nc ; c++, h += hstep) hsv2rgb(h, hsvlut[c]) ;
    }

    unsigned short minv, maxv ;
    int i, j ;

    minv = 0xffff ;
    maxv = 0 ;

    const uchar *ppl = img.data() ;
    unsigned short *pp = (unsigned short *)ppl ;

    for ( i=0 ; i<h ; i++, ppl += lw )
        for ( j=0, pp = (unsigned short *)ppl ; j<w ; j++, pp++ )
        {
            if ( *pp == 0 ) continue ;
            maxv = qMax(*pp, maxv) ;
            minv = qMin(*pp, minv) ;
        }

    QImage image(w, h, QImage::Format_RGB32) ;

    for( i=0 ; i<h ; i++ )
    {
        uchar *dst = image.scanLine(i) ;
        unsigned short *src = (unsigned short *)img.data() + img.width() * i ;

        for( j=0 ; j<w ; j++ )
        {
            unsigned short val = *src++ ;

            if ( val == 0 )
            {
                *(QRgb *)dst = Qt::black ;
                dst += 3 ;
                *dst++ = 255 ;

                continue ;
            }
            else val = (nc-1)*float((val - minv)/float(maxv - minv)) ;

            const QRgb &clr = hsvlut[val] ;

            *(QRgb *)dst = clr ;
            dst += 3 ;
            *dst++ = 255 ;
        }
    }

    return image ;

}



void write_depth(Image& im, const std::string &fpath) {
    QImage qim = imageToQImage(im) ;
    qim.save(QString::fromStdString(fpath)) ;

}

int main(int argc, char **argv)
{
    TestApplication app("skeleton", argc, argv) ;

    ScenePtr model(new Scene) ;

    //  model->load(TestApplication::data() + "/models/RiggedFigure.dae", Node::IMPORT_ANIMATIONS | Node::IMPORT_SKELETONS) ;

    model->load( "/home/malasiot/source/human_tracking/pose_estimation/data/models/human-cmu.dae", Node::IMPORT_ANIMATIONS | Node::IMPORT_SKELETONS) ;
    replace_texture(model, "/home/malasiot/source/human_tracking/pose_estimation/data/parts.png") ;
    set_geom_visibility(model, false) ;

    float r = 7 ;
    Vector3f c{0, 0.75, 0.0};
    unsigned int width = 480, height = 480 ;
    PerspectiveCamera *pcam = new PerspectiveCamera(1, // aspect ratio
                                                    50*M_PI/180,   // fov
                                                    0.0001,        // zmin
                                                    10*r           // zmax
                                                    ) ;


    //    OrthographicCamera *pcam = new OrthographicCamera(-0.6*r, 0.6*r, 0.6*r, -0.6*r,0.0001, 10*r) ;

    CameraPtr cam(pcam) ;

    cam->setBgColor({1, 0, 0, 1});

    // position camera to look at the center of the object

    //  pcam->viewSphere(c, r) ;
    pcam->lookAt(c + Vector3f{0, 0, 4*r}, c, {0, 1, 0}) ;

    // set camera viewpot

    pcam->setViewport(width, height)  ;

    OffscreenRenderer rdr(QSize(width, height));
    rdr.render(model, cam) ;
    auto clr = rdr.getImage() ;
    clr.saveToPNG("/tmp/oo.png") ;

    QImage im(clr.data(), clr.width(), clr.height(), QImage::Format_RGBA8888) ;
    im.save("/tmp/clr.png") ;

    auto depth = rdr.getDepthBuffer(0.0001, 10*r) ;


     depth.saveToPNG("/tmp/oo.png") ;

    write_depth(depth, "/tmp/depth.png") ;



    /*
    SceneViewer::initDefaultGLContext();
    SceneViewer *viewer = new SceneViewer(model);
    viewer->setDefaultCamera() ;
    viewer->startAnimations() ;

    QMainWindow window ;
    window.setCentralWidget(viewer) ;
    window.resize(512, 512) ;
    window.show() ;

    return app.exec();
    */
}
