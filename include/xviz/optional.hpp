#ifndef XVIZ_UTIL_OPTIONAL_HPP
#define XVIZ_UTIL_OPTIONAL_HPP

/* just wrapping 3rdparty implementation of optional to support c++11 */
/* it fallbacks to std::optional for c++17 and above */

#include <xviz/detail/optional.hpp>

namespace xviz {
    using nonstd::optional ;
}

#endif
