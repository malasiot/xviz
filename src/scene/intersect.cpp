#include <xviz/scene/detail/intersect.hpp>

#include <iostream>
using namespace Eigen ;
using namespace std ;

namespace xviz { namespace detail {


AABB::~AABB() {}

bool rayIntersectsAABB(const Ray &r, const AABB &box, float &t) {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (box.bounds_[r.sign_[0]].x() - r.orig_.x()) * r.invdir_.x();
    tmax = (box.bounds_[1-r.sign_[0]].x() - r.orig_.x()) * r.invdir_.x();
    tymin = (box.bounds_[r.sign_[1]].y() - r.orig_.y()) * r.invdir_.y();
    tymax = (box.bounds_[1-r.sign_[1]].y() - r.orig_.y()) * r.invdir_.y();

    if ((tmin > tymax) || (tymin > tmax)) return false;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    tzmin = (box.bounds_[r.sign_[2]].z() - r.orig_.z()) * r.invdir_.z();
    tzmax = (box.bounds_[1-r.sign_[2]].z() - r.orig_.z()) * r.invdir_.z();

    if ((tmin > tzmax) || (tzmin > tmax)) return false;

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    t = tmin ;
    if ( t < 0 ) {
        t = tmax ;
        if ( t < 0 ) return false;
    }

    return true;
}

bool rayIntersectsTriangle(const Ray ray,
                           const Vector3f &v0,
                           const Vector3f &v1,
                           const Vector3f &v2,
                           bool back_face_culling,
                           float &t)
{
    const float eps = 1.0e-30;
    Vector3f edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    h = ray.dir_.cross(edge2);
    a = edge1.dot(h);
    f = 1/a;

    if ( back_face_culling ) {
        if ( a < eps ) return false;
        s = ray.orig_ - v0;
        u =  ( s.dot(h) );
        if ( u < 0.0 || u > a ) return false;
        q = s.cross(edge1);
        v = ray.dir_.dot(q) ;
        if (v < 0.0 || u + v > a) return false;

        // At this stage we can compute t to find out where the intersection point is on the line.
        t = f * edge2.dot(q) ;
    }
    else {
        if ( a > -eps && a < eps ) return false;
        s = ray.orig_ - v0;
        u = f * ( s.dot(h) );
        if ( u < 0.0 || u > 1.0 ) return false;
        q = s.cross(edge1);
        v = f * ray.dir_.dot(q) ;
        if (v < 0.0 || u + v > 1.0)
            return false;

        // At this stage we can compute t to find out where the intersection point is on the line.
        t = f * edge2.dot(q);

    }

    return t > eps ;

}

bool rayIntersectsSphere(const Ray &ray, const Vector3f &center, float radius, float &t) {

    float t0, t1; // solutions for t if the ray intersects

    float radius2 = radius * radius ;
    Vector3f L = center - ray.orig_;
    float tca = L.dot(ray.dir_);
    // if (tca < 0) return false;
    float d2 = L.dot(L) - tca * tca;
    if (d2 > radius2) return false;
    float thc = sqrt(radius2 - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    if (t0 > t1) std::swap(t0, t1);

    t = t0 ;
    if ( t < 0 ) {
        t = t1; // if t0 is negative, let's use t1 instead
        if ( t < 0 ) return false; // both t0 and t1 are negative
    }

    return true;
}


/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap(float boxcenter[3],      */
/*          float boxhalfsize[3],float triverts[3][3]); */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/

// https://gist.github.com/Philipp-M/e5747bd5a4e264ab143824059d21c120

inline void findMinMax(float x0, float x1, float x2, float &min, float &max) {
    min = max = x0;
    if (x1 < min) min = x1;
    if (x1 > max) max = x1;
    if (x2 < min) min = x2;
    if (x2 > max) max = x2;
}

inline bool planeBoxOverlap(const Vector3f &normal, const Vector3f &vert, const Vector3f &maxbox) {
    Vector3f vmin, vmax;
    float v;
    for (int q = 0; q < 3; q++) {
        v = vert[q];
        if (normal[q] > 0.0f) {
            vmin[q] = -maxbox[q] - v;
            vmax[q] = maxbox[q] - v;
        } else {
            vmin[q] = maxbox[q] - v;
            vmax[q] = -maxbox[q] - v;
        }
    }
    if ( normal.dot(vmin) > 0.0f) return false;
    if ( normal.dot(vmax) >= 0.0f) return true;

    return false;
}

/*======================== X-tests ========================*/


inline bool axisTestX01(float a, float b, float fa, float fb, const Vector3f &v0,
                        const Vector3f &v2, const Vector3f &boxhalfsize, float &rad, float &min,
                        float &max, float &p0, float &p2) {
    p0 = a * v0.y() - b * v0.z();
    p2 = a * v2.y() - b * v2.z();
    if (p0 < p2) {
        min = p0;
        max = p2;
    } else {
        min = p2;
        max = p0;
    }
    rad = fa * boxhalfsize.y() + fb * boxhalfsize.z();
    if (min > rad || max < -rad)
        return false;
    return true;
}


inline bool axisTestX2(float a, float b, float fa, float fb, const Vector3f &v0,
                       const Vector3f &v1, const Vector3f &boxhalfsize, float &rad, float &min,
                       float &max, float &p0, float &p1) {
    p0 = a * v0.y() - b * v0.z();
    p1 = a * v1.y() - b * v1.z();
    if (p0 < p1) {
        min = p0;
        max = p1;
    } else {
        min = p1;
        max = p0;
    }
    rad = fa * boxhalfsize.y() + fb * boxhalfsize.z();
    if (min > rad || max < -rad)
        return false;
    return true;
}

/*======================== Y-tests ========================*/

inline bool axisTestY02(float a, float b, float fa, float fb, const Vector3f &v0,
                        const Vector3f &v2, const Vector3f &boxhalfsize, float &rad, float &min,
                        float &max, float &p0, float &p2) {
    p0 = -a * v0.x() + b * v0.z();
    p2 = -a * v2.x() + b * v2.z();
    if (p0 < p2) {
        min = p0;
        max = p2;
    } else {
        min = p2;
        max = p0;
    }
    rad = fa * boxhalfsize.x() + fb * boxhalfsize.z();
    if (min > rad || max < -rad)
        return false;
    return true;
}

inline bool axisTestY1(float a, float b, float fa, float fb, const Vector3f &v0,
                       const Vector3f &v1, const Vector3f &boxhalfsize, float &rad, float &min,
                       float &max, float &p0, float &p1) {
    p0 = -a * v0.x() + b * v0.z();
    p1 = -a * v1.x() + b * v1.z();
    if (p0 < p1) {
        min = p0;
        max = p1;
    } else {
        min = p1;
        max = p0;
    }
    rad = fa * boxhalfsize.x() + fb * boxhalfsize.z();
    if (min > rad || max < -rad)
        return false;
    return true;
}


/*======================== Z-tests ========================*/
inline bool axisTestZ12(float a, float b, float fa, float fb, const Vector3f &v1, const Vector3f &v2,
                        const Vector3f &boxhalfsize, float &rad, float min, float max, float &p1, float &p2) {
    p1 = a * v1.x() - b * v1.y() ;
    p2 = a * v2.x() - b * v2.y();

    if ( p2 < p1 ) {
        min = p2 ;
        max = p1 ;
    } else {
        min = p1 ;
        max = p2 ;
    }
    rad = fa * boxhalfsize.x() + fb * boxhalfsize.y();
    if (min > rad || max < -rad)
        return false;
    return true;
}

inline bool axisTestZ0(float a, float b, float fa, float fb, const Vector3f &v0,
                       const Vector3f &v1, const Vector3f &boxhalfsize, float &rad, float &min,
                       float &max, float &p0, float &p1) {
    p0 = a * v0.x() - b * v0.y();
    p1 = a * v1.x() - b * v1.y();
    if (p0 < p1) {
        min = p0;
        max = p1;
    } else {
        min = p1;
        max = p0;
    }
    rad = fa * boxhalfsize.x() + fb * boxhalfsize.y();
    if (min > rad || max < -rad)
        return false;
    return true;
}

bool triangleInsideBox(const Eigen::Vector3f &tv0, const Eigen::Vector3f &tv1,
                       const Eigen::Vector3f &tv2, const Eigen::Vector3f &boxcenter, const Eigen::Vector3f &boxhalfsize) {
    Vector3f v0, v1, v2;
    float min, max ;

    /* This is the fastest branch on Sun */
    /* move everything so that the boxcenter is in (0,0,0) */
    v0 = tv0 - boxcenter;
    v1 = tv1 - boxcenter;
    v2 = tv2 - boxcenter;

    if ( v0.x() < -boxhalfsize.x() || v0.x() > boxhalfsize.x() ) return false ;
    if ( v1.x() < -boxhalfsize.x() || v1.x() > boxhalfsize.x() ) return false ;
    if ( v2.x() < -boxhalfsize.x() || v2.x() > boxhalfsize.x() ) return false ;

    if ( v0.y() < -boxhalfsize.y() || v0.y() > boxhalfsize.y() ) return false ;
    if ( v1.y() < -boxhalfsize.y() || v1.y() > boxhalfsize.y() ) return false ;
    if ( v2.y() < -boxhalfsize.y() || v2.y() > boxhalfsize.y() ) return false ;

    if ( v0.z() < -boxhalfsize.z() || v0.z() > boxhalfsize.z() ) return false ;
    if ( v1.z() < -boxhalfsize.z() || v1.z() > boxhalfsize.z() ) return false ;
    if ( v2.z() < -boxhalfsize.z() || v2.z() > boxhalfsize.z() ) return false ;

    return true ;
}

bool triangleOutsideBox(const Eigen::Vector3f &tv0, const Eigen::Vector3f &tv1,
                        const Eigen::Vector3f &tv2, const Eigen::Vector3f &boxcenter, const Eigen::Vector3f &boxhalfsize) {

    float min, max ;
    Vector3f v0 = tv0 - boxcenter;
    Vector3f v1 = tv1 - boxcenter;
    Vector3f v2 = tv2 - boxcenter;

    /* test in X-direction */
    findMinMax(v0.x(), v1.x(), v2.x(), min, max);
    if (min > boxhalfsize.x() || max < -boxhalfsize.x())
        return true;

    /* test in Y-direction */
    findMinMax(v0.y(), v1.y(), v2.y(), min, max);
    if (min > boxhalfsize.y() || max < -boxhalfsize.y())
        return true;

    /* test in Z-direction */
    findMinMax(v0.z(), v1.z(), v2.z(), min, max);
    if (min > boxhalfsize.z() || max < -boxhalfsize.z())
        return true;

    return false;

}

bool triangleIntersectsBox(const Eigen::Vector3f &tv0, const Eigen::Vector3f &tv1,
                           const Eigen::Vector3f &tv2, const Eigen::Vector3f &boxcenter, const Eigen::Vector3f &boxhalfsize) {
    /*    use separating axis theorem to test overlap between triangle and box */
    /*    need to test for overlap in these directions: */
    /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
    /*       we do not even need to test these) */
    /*    2) normal of the triangle */
    /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
    /*       this gives 3x3=9 more tests */
    Vector3f v0, v1, v2;
    float min, max, p0, p1, p2, rad, fex, fey, fez;
    Vector3f normal, e0, e1, e2;

    /* This is the fastest branch on Sun */
    /* move everything so that the boxcenter is in (0,0,0) */
    v0 = tv0 - boxcenter;
    v1 = tv1 - boxcenter;
    v2 = tv2 - boxcenter;

    /* compute triangle edges */
    e0 = v1 - v0;
    e1 = v2 - v1;
    e2 = v0 - v2;



    /* Bullet 3:  */
    /*  test the 9 tests first (this was faster) */
    fex = fabsf(e0.x());
    fey = fabsf(e0.y());
    fez = fabsf(e0.z());

    if (!axisTestX01(e0.z(), e0.y(), fez, fey, v0, v2, boxhalfsize, rad, min, max, p0, p2))
        return false;
    if (!axisTestY02(e0.z(), e0.x(), fez, fex, v0, v2, boxhalfsize, rad, min, max, p0, p2))
        return false;
    if (!axisTestZ12(e0.y(), e0.x(), fey, fex, v1, v2, boxhalfsize, rad, min, max, p1, p2))
        return false;

    fex = fabsf(e1.x());
    fey = fabsf(e1.y());
    fez = fabsf(e1.z());

    if (!axisTestX01(e1.z(), e1.y(), fez, fey, v0, v2, boxhalfsize, rad, min, max, p0, p2))
        return false;
    if (!axisTestY02(e1.z(), e1.x(), fez, fex, v0, v2, boxhalfsize, rad, min, max, p0, p2))
        return false;
    if (!axisTestZ0(e1.y(), e1.x(), fey, fex, v0, v1, boxhalfsize, rad, min, max, p0, p1))
        return false;

    fex = fabsf(e2.x());
    fey = fabsf(e2.y());
    fez = fabsf(e2.z());

    if (!axisTestX2(e2.z(), e2.y(), fez, fey, v0, v1, boxhalfsize, rad, min, max, p0, p1))
        return false;
    if (!axisTestY1(e2.z(), e2.x(), fez, fex, v0, v1, boxhalfsize, rad, min, max, p0, p1))
        return false;
    if (!axisTestZ12(e2.y(), e2.x(), fey, fex, v1, v2, boxhalfsize, rad, min, max, p1, p2))
        return false;


    /* Bullet 1: */
    // Test if triangle is completely outside box

    /* test in X-direction */
    findMinMax(v0.x(), v1.x(), v2.x(), min, max);
    if (min > boxhalfsize.x() || max < -boxhalfsize.x())
        return false;

    /* test in Y-direction */
    findMinMax(v0.y(), v1.y(), v2.y(), min, max);
    if (min > boxhalfsize.y() || max < -boxhalfsize.y())
        return false;

    /* test in Z-direction */
    findMinMax(v0.z(), v1.z(), v2.z(), min, max);
    if (min > boxhalfsize.z() || max < -boxhalfsize.z())
        return false;

    /* Bullet 2: */
    /*  test if the box intersects the plane of the triangle */
    /*  compute plane equation of triangle: normal*x+d=0 */
    normal = e0.cross(e1) ;
    if (!planeBoxOverlap(normal, v0, boxhalfsize))
        return false;


    return true; /* box and triangle overlaps */
}

bool rayIntersectsPoint(const Ray &ray, const Vector3f &p, float thresh_square, float &t) {
    Vector3f o = ray.origin(), d = ray.dir(), po = p-o ;

    float dist = d.cross(po).squaredNorm() ;
    if ( dist < thresh_square ) return false ;

    t = po.dot(d) ;
    if ( t < 0 ) return false ;

    return true ;
}

bool rayIntersectsLine(const Ray &ray, const Vector3f &p1, const Vector3f &p2, float thresh, float &t)
{
    Vector3f da = ray.dir();
    Vector3f db = p2 - p1 ;
    Vector3f dc = p1 - ray.origin() ;

    Vector3f cab = da.cross(db) ;

    if ( fabs(dc.dot(cab)) >= thresh ) return false ;

    t = dc.cross(db).dot(cab) / cab.squaredNorm() ;

    if ( t >= 0.0  )	{

        Vector3f intersection = ray.origin() + t * da;

        // See if this lies on the segment
        if ( (intersection - p1).squaredNorm() + (intersection - p2).squaredNorm() - (p1 - p2).squaredNorm() <= thresh)
            return true;
    }

    return false;
}

static bool intersectsDisk(const Vector3f &p, const Vector3f &d, float radius, float height, float &t) {
    if ( fabs(d.z()) < std::numeric_limits<float>::min() ) return false ; // parallel to disk
    t = (height - p.z()) / d.z() ;
    if ( t < 0 ) return false ;

    Vector3f ip = p + t * d ;

    if ( ip.x()*ip.x() + ip.y()*ip.y() > radius * radius ) return false ;
    cout << t << endl ;
    return true;
}

bool rayIntersectsCylinder(const Ray &ray, float radius, float height, float &t) {
    float h2 = height/2.0 ;
    Vector3f d = ray.dir(), p = ray.origin() ;
    float c =  p.x() * p.x() + p.y() * p.y() - radius * radius ;
    float b = 2 * ( p.x() * d.x() + p.y() * d.y() ) ;
    float a = d.x() * d.x() + d.y() * d.y() ;

    float delta = b*b - 4*(a*c);
    if ( delta < 0 ) return false ;
    if ( fabs(delta) < std::numeric_limits<float>::min() ) return false ;

    bool has_intersection = false ;

    t = (-b - sqrt(delta))/(2*a);

    if ( t > 0.0 ) {
        float iz = p.z() + t*d.z() ;
        has_intersection = ( fabs(iz) < h2 ) ;

    }

    if ( !has_intersection ) {
        t = std::numeric_limits<float>::max() ;
    }

    float t1, t2 ;

    if ( intersectsDisk(p, d, radius, -h2, t1) ) { // top cap
        if ( t1 < t ) {
            t = t1 ;
            has_intersection = true ;
        }
    }

    if ( intersectsDisk(p, d, radius, h2, t2) ) { // bottom cap
        if ( t2 < t ) {
            t = t2 ;
            has_intersection = true ;
        }
    }


    return has_intersection ;
}


#define     EQN_EPS     1e-90

#define	IsZero(x)	((x) > -EQN_EPS && (x) < EQN_EPS)

#ifndef CBRT
#define     cbrt(x)  ((x) > 0.0 ? pow((double)(x), 1.0/3.0) : \
                     ((x) < 0.0 ? -pow((double)-(x), 1.0/3.0) : 0.0))
#endif

int
SolveQuadric(double c[3], double s[2]) {
    double p, q, D;

    /* normal form: x^2 + px + q = 0 */

    p = c[ 1 ] / (2 * c[ 2 ]);
    q = c[ 0 ] / c[ 2 ];

    D = p * p - q;

    if (IsZero(D)) {
    s[ 0 ] = - p;
    return 1;
    }
    else if (D > 0) {
    double sqrt_D = sqrt(D);

    s[ 0 ] =   sqrt_D - p;
    s[ 1 ] = - sqrt_D - p;
    return 2;
    }
    else /* if (D < 0) */
        return 0;
}


int
SolveCubic(double c[4], double s[3]) {
    int     i, num;
    double  sub;
    double  A, B, C;
    double  sq_A, p, q;
    double  cb_p, D;

    /* normal form: x^3 + Ax^2 + Bx + C = 0 */

    A = c[ 2 ] / c[ 3 ];
    B = c[ 1 ] / c[ 3 ];
    C = c[ 0 ] / c[ 3 ];

    /*  substitute x = y - A/3 to eliminate quadric term:
    x^3 +px + q = 0 */

    sq_A = A * A;
    p = 1.0/3 * (- 1.0/3 * sq_A + B);
    q = 1.0/2 * (2.0/27 * A * sq_A - 1.0/3 * A * B + C);

    /* use Cardano's formula */

    cb_p = p * p * p;
    D = q * q + cb_p;

    if (IsZero(D)) {
        if (IsZero(q)) { /* one triple solution */
            s[ 0 ] = 0;
            num = 1;
        }
    else { /* one single and one double solution */
        double u = cbrt(-q);
        s[ 0 ] = 2 * u;
        s[ 1 ] = - u;
        num = 2;
    }
    }
    else if (D < 0) { /* Casus irreducibilis: three real solutions */
        double phi = 1.0/3 * acos(-q / sqrt(-cb_p));
        double t = 2 * sqrt(-p);

        s[ 0 ] =   t * cos(phi);
        s[ 1 ] = - t * cos(phi + M_PI / 3);
        s[ 2 ] = - t * cos(phi - M_PI / 3);
        num = 3;
    }
    else { /* one real solution */
        double sqrt_D = sqrt(D);
        double u = cbrt(sqrt_D - q);
        double v = - cbrt(sqrt_D + q);

        s[ 0 ] = u + v;
        num = 1;
    }

    /* resubstitute */

    sub = 1.0/3 * A;

    for (i = 0; i < num; ++i)
    s[ i ] -= sub;

    return num;
}



int
SolveQuartic(double c[5], double s[4]) {
    double  coeffs[4];
    double  z, u, v, sub;
    double  A, B, C, D;
    double  sq_A, p, q, r;
    int     i, num;

    /* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */

    A = c[ 3 ] / c[ 4 ];
    B = c[ 2 ] / c[ 4 ];
    C = c[ 1 ] / c[ 4 ];
    D = c[ 0 ] / c[ 4 ];

    /*  substitute x = y - A/4 to eliminate cubic term:
    x^4 + px^2 + qx + r = 0 */

    sq_A = A * A;
    p = - 3.0/8 * sq_A + B;
    q = 1.0/8 * sq_A * A - 1.0/2 * A * B + C;
    r = - 3.0/256*sq_A*sq_A + 1.0/16*sq_A*B - 1.0/4*A*C + D;

    if (IsZero(r)) {
        /* no absolute term: y(y^3 + py + q) = 0 */

        coeffs[ 0 ] = q;
        coeffs[ 1 ] = p;
        coeffs[ 2 ] = 0;
        coeffs[ 3 ] = 1;

        num = SolveCubic(coeffs, s);

        s[ num++ ] = 0;
    }
    else {
        /* solve the resolvent cubic ... */

        coeffs[ 0 ] = 1.0/2 * r * p - 1.0/8 * q * q;
        coeffs[ 1 ] = - r;
        coeffs[ 2 ] = - 1.0/2 * p;
        coeffs[ 3 ] = 1;

        (void) SolveCubic(coeffs, s);

        /* ... and take the one real solution ... */

        z = s[ 0 ];

        /* ... to build two quadric equations */

        u = z * z - r;
        v = 2 * z - p;

        if (IsZero(u))
            u = 0;
        else if (u > 0)
            u = sqrt(u);
        else
            return 0;

        if (IsZero(v))
            v = 0;
        else if (v > 0)
            v = sqrt(v);
        else
            return 0;

        coeffs[ 0 ] = z - u;
        coeffs[ 1 ] = q < 0 ? -v : v;
        coeffs[ 2 ] = 1;

        num = SolveQuadric(coeffs, s);

        coeffs[ 0 ]= z + u;
        coeffs[ 1 ] = q < 0 ? v : -v;
        coeffs[ 2 ] = 1;

        num += SolveQuadric(coeffs, s + num);
    }

    /* resubstitute */

    sub = 1.0/4 * A;

    for (i = 0; i < num; ++i)
        s[ i ] -= sub;

    return num;
}

bool rayIntersectsTorus(const Ray& ray, float a, float b, float& tmin)  {

    const auto &o = ray.origin() ;
    const auto &d = ray.dir() ;

    float x1 = o.x(), y1 = o.y(), z1 = o.z();
    float d1 = d.x(), d2 = d.y(), d3 = d.z();

    double coeffs[5], roots[4];

    // define the coefficients of the quartic equation

    float sum_d_sqrd 	= d1 * d1 + d2 * d2 + d3 * d3;
    float e			= x1 * x1 + y1 * y1 + z1 * z1 - a * a - b * b;
    float f			= x1 * d1 + y1 * d2 + z1 * d3;
    float four_a_sqrd	= 4.0 * a * a;

    coeffs[0] = e * e - four_a_sqrd * (b * b - y1 * y1); 	// constant term
    coeffs[1] = 4.0 * f * e + 2.0 * four_a_sqrd * y1 * d2;
    coeffs[2] = 2.0 * sum_d_sqrd * e + 4.0 * f * f + four_a_sqrd * d2 * d2;
    coeffs[3] = 4.0 * sum_d_sqrd * f;
    coeffs[4] = sum_d_sqrd * sum_d_sqrd;  					// coefficient of t^4

    // find roots of the quartic equation

    int num_real_roots = SolveQuartic(coeffs, roots);

    bool	intersected = false;
    float 	t 		 	= std::numeric_limits<float>::max();

    if (num_real_roots == 0)  // ray misses the torus
        return(false);

    // find the smallest root greater than kEpsilon, if any
    // the roots array is not sorted

    for (int j = 0; j < num_real_roots; j++)
        if (roots[j] > 1.e-6 ) {
            intersected = true;
            if (roots[j] < t)
                t = roots[j];
        }

    if(!intersected)
        return (false);

    tmin = t;

    return (true);
}

}}
