#include <vector>
#include <QPainter>

#include <xviz/annotation.hpp>

class AnnotationPainter {

public:

    AnnotationPainter(const std::vector<xviz::Annotation> &an): annotations_(an) {}

    void paint(QPainter &painter, const xviz::Matrix2d &mat);

private:

    void paintLabels(QPainter &c, const xviz::Matrix2d &m, const xviz::LabelAnnotation &la) ;
    void paintShapes(QPainter &c, const xviz::Matrix2d &m, const xviz::ShapeAnnotation &la);
    void paintMarkers(QPainter &c, const xviz::Matrix2d &m, const xviz::MarkerAnnotation &ma);


    const std::vector<xviz::Annotation> &annotations_ ;
};
