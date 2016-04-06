#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>

#include <cassert>
#include <string>

using namespace mbgl;


FeatureIndex::FeatureIndex() {}

void FeatureIndex::insert(const GeometryCollection& geometries, std::size_t index,
        const std::string& sourceLayerName, const std::string& bucketName) {

    for (auto& ring : geometries) {

        float minX = std::numeric_limits<float>::infinity();
        float minY = std::numeric_limits<float>::infinity();
        float maxX = -std::numeric_limits<float>::infinity();
        float maxY = -std::numeric_limits<float>::infinity();
        for (auto& p : ring) {
            const float x = p.x;
            const float y = p.y;
            minX = util::min(minX, x);
            minY = util::min(minY, y);
            maxX = util::max(maxX, x);
            maxY = util::max(maxY, y);
        }

        treeBoxes.emplace_back(
            TreeBox {
                TreePoint { minX, minY },
                TreePoint { maxX, maxY }
            },
            IndexedSubfeature { index, sourceLayerName, bucketName, treeBoxes.size() }
        );
    }
}

void FeatureIndex::loadTree() {
    tree.insert(treeBoxes.begin(), treeBoxes.end());
}

void FeatureIndex::query(
        std::unordered_map<std::string, std::vector<std::string>>& result,
        const GeometryCollection& queryGeometry,
        double,
        const GeometryTile& geometryTile) {

    float minX = std::numeric_limits<float>::infinity();
    float minY = std::numeric_limits<float>::infinity();
    float maxX = -std::numeric_limits<float>::infinity();
    float maxY = -std::numeric_limits<float>::infinity();

    for (auto& ring : queryGeometry) {
        for (auto& p : ring) {
            minX = util::min<float>(minX, p.x);
            minY = util::min<float>(minY, p.y);
            maxX = util::max<float>(maxX, p.x);
            maxY = util::max<float>(maxY, p.y);
        }
    }

    TreeBox queryBox = {
        TreePoint { minX, minY },
        TreePoint { maxX, maxY }
    };

    std::vector<FeatureTreeBox> matchingBoxes;
    tree.query(bgi::intersects(queryBox), std::back_inserter(matchingBoxes));

    std::sort(matchingBoxes.begin(), matchingBoxes.end(), [](FeatureTreeBox& a, FeatureTreeBox& b) {
        return std::get<1>(b).sortIndex - std::get<1>(a).sortIndex;
    });

    for (auto& matchingBox : matchingBoxes) {
        auto& indexedFeature = std::get<1>(matchingBox);

        auto sourceLayer = geometryTile.getLayer(indexedFeature.sourceLayerName);
        assert(sourceLayer);
        auto feature = sourceLayer->getFeature(indexedFeature.index);
        assert(feature);

        auto& bucketName = indexedFeature.bucketName;
        auto& layerResult = result[bucketName];

        layerResult.push_back(indexedFeature.sourceLayerName);
    }

}
