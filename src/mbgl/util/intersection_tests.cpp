#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {
namespace util {

bool multiPolygonIntersectsBufferedMultiPoint(const GeometryCollection&, const GeometryCollection&, float) {
    return true;
}

bool multiPolygonIntersectsBufferedMultiLine(const GeometryCollection&, const GeometryCollection&, float) {
    return true;
}

bool multiPolygonIntersectsMultiPolygon(const GeometryCollection&, const GeometryCollection&) {
    return true;
}

}
}
