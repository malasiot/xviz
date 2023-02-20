#include <xviz/scene/text.hpp>
#include "text_item.hpp"

using namespace std ;
using namespace Eigen ;

namespace xviz {

Text::Text(const std::string &t, const Font &f): impl_(new impl::TextItem(t, f)) {
}

void Text::render(float x, float y, const Vector3f &clr) {
    impl_->render(x, y, clr) ;
}



}
