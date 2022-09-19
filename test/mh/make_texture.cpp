#include "bvh.hpp"

#include "mhx2_importer.hpp"
#include "mhx2_viz_scene.hpp"

#include <cvx/misc/format.hpp>
#include <cvx/misc/strings.hpp>

#include <opencv2/opencv.hpp>

using namespace std ;
using namespace Eigen ;
using namespace cvx ;

struct Part {
    uint r_, g_, b_ ;
    std::vector<size_t> faces_ ;
};

const size_t img_size = 4096 ;


void load_parts(const std::string &fpath, std::map<std::string, Part> &parts) {
    ifstream strm(fpath) ;
    string line, name ;
    std::set<size_t> colors ;

    while (strm) {
       std::getline(strm, line) ;
       if ( line.empty() ) break ;
       std::stringstream ss(line) ;
       Part p ;
       ss >> name >> p.r_ >> p.g_ >> p.b_ ;

       if ( !startsWith(name, "Material") ) continue ;

       uint32_t rgb =  (((uint32_t)p.r_ << 16) & 0x00FF0000) |  (((uint32_t)p.g_ << 8) & 0x0000FF00) | ( (uint32_t)p.b_ & 0x000000FF ) | 0xFF000000 ;

       if ( colors.count(rgb) ) {
           cerr << "duplicate colors: " << name << endl ;
       } else
           colors.insert(rgb) ;


       std::copy(std::istream_iterator<size_t>(ss),
                 std::istream_iterator<size_t>(),
                 std::back_inserter(p.faces_));

       parts.emplace(name, std::move(p)) ;
    }
}

void make_texture(const MHX2Model &model, const std::map<std::string, Part> &parts, cv::Mat &texture ) {
    std::map<size_t, std::string> face_part_map ;
    for( const auto &mp: parts ) {
        string name = mp.first ;
        for( const auto &face: mp.second.faces_ )
            face_part_map[face] = name ;
    }

    auto it = model.geometries_.find("Human_cmu:Body") ;
    if ( it == model.geometries_.end() ) {
        cerr << "mesh not found" <<endl ;
        return ;
    }

    const MHX2Mesh &geom = it->second.mesh_;


    for( size_t i=0 ; i< geom.uv_faces_.size() ; i++ ) {
        const auto &face = geom.uv_faces_[i] ;
        size_t idx0 = face.indices_[0] ;
        size_t idx1 = face.indices_[1] ;
        size_t idx2 = face.indices_[2] ;
        size_t idx3 = face.indices_[3] ;

        auto it = face_part_map.find(i) ;

        if ( it != face_part_map.end() ) {
            const string &material_name = it->second ;

            auto pit = parts.find(material_name) ;
            if ( pit != parts.end() ) {
                const Part &part = pit->second ;

                cout << i << ' ' << material_name << endl ;
                const Vector2f &coord0 = geom.uv_coords_[idx0] ;
                const Vector2f &coord1 = geom.uv_coords_[idx1] ;
                const Vector2f &coord2 = geom.uv_coords_[idx2] ;
                const Vector2f &coord3 = geom.uv_coords_[idx3] ;

                std::vector<cv::Point2i> pts;
                pts.emplace_back(std::floor(coord0.x() * img_size), std::floor(coord0.y() * img_size)) ;
                pts.emplace_back(std::floor(coord1.x() * img_size), std::floor(coord1.y() * img_size)) ;
                pts.emplace_back(std::floor(coord2.x() * img_size), std::floor(coord2.y() * img_size)) ;
                pts.emplace_back(std::floor(coord3.x() * img_size), std::floor(coord3.y() * img_size)) ;

                cv::fillConvexPoly(texture, pts, cv::Scalar(part.r_, part.g_, part.b_, 255));
            }
        }
    }

   cv::rotate(texture, texture, cv::ROTATE_90_COUNTERCLOCKWISE);
}

int main(int argc, char *argv[]) {

    Mhx2Importer importer ;
    importer.load("/home/malasiot/Downloads/human-cmu.mhx2", "Human_cmu:Body") ;

    std::map<std::string, Part> parts ;
    load_parts("/home/malasiot/Downloads/parts.txt", parts) ;

    cv::Mat texture(img_size, img_size, CV_8UC4, cv::Scalar(0, 0, 0, 0)) ;

    make_texture(importer.getModel(), parts, texture) ;
    cv::imwrite("/home/malasiot/Downloads/parts_texture.png", texture) ;

    ofstream ps("/home/malasiot/Downloads/parts.txt") ;

}
