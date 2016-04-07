#ifndef MBGL_GEOMETRY_FEATURE_INDEX
#define MBGL_GEOMETRY_FEATURE_INDEX

#include <mbgl/tile/geometry_tile.hpp>

#include <vector>
#include <string>
#include <unordered_map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wshadow"
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-register"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#pragma GCC diagnostic pop

namespace mbgl {

class Style;
enum class TranslateAnchorType : bool;

class IndexedSubfeature {
    public:
        IndexedSubfeature(std::size_t index_, std::string sourceLayerName_, std::string bucketName_, size_t sortIndex_) :
            index(index_), sourceLayerName(sourceLayerName_), bucketName(bucketName_), sortIndex(sortIndex_) {};
        std::size_t index;
        std::string sourceLayerName;
        std::string bucketName;
        size_t sortIndex;
};

namespace bg = boost::geometry;
namespace bgm = bg::model;
namespace bgi = bg::index;
typedef bgm::point<float, 2, bg::cs::cartesian> TreePoint;
typedef bgm::box<TreePoint> TreeBox;
typedef std::pair<TreeBox, IndexedSubfeature> FeatureTreeBox;
typedef bgi::rtree<FeatureTreeBox, bgi::linear<16, 4>> FeatureTree;

class FeatureIndex {
    public:
        FeatureIndex();

        void insert(const GeometryCollection&, std::size_t index, const std::string& sourceLayerName, const std::string& bucketName);
        void loadTree();

        static optional<GeometryCollection> translateQueryGeometry(
                const GeometryCollection& queryGeometry,
                const std::array<float, 2>& translate,
                const TranslateAnchorType,
                const float bearing,
                const float pixelsToTileUnits);

        void query(
                std::unordered_map<std::string, std::vector<std::string>>& result,
                const GeometryCollection& queryGeometry,
                double scale,
                const GeometryTile& geometryTile,
                const Style&);

    private:
        std::vector<FeatureTreeBox> treeBoxes;
        FeatureTree tree;

};
}

#endif
