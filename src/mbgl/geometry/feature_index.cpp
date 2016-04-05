#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>

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
            IndexedSubfeature { index, sourceLayerName, bucketName }
        );
    }
}

void FeatureIndex::loadTree() {
    tree.insert(treeBoxes.begin(), treeBoxes.end());
}

void FeatureIndex::query(std::unordered_map<std::string, std::vector<std::string>>& result) {
    float minX = -std::numeric_limits<float>::infinity();
    float minY = -std::numeric_limits<float>::infinity();
    float maxX = std::numeric_limits<float>::infinity();
    float maxY = std::numeric_limits<float>::infinity();

    TreeBox queryBox = {
        TreePoint { minX, minY },
        TreePoint { maxX, maxY }
    };

    std::vector<FeatureTreeBox> matchingBoxes;
    tree.query(bgi::intersects(queryBox), std::back_inserter(matchingBoxes));

    for (auto& matchingBox : matchingBoxes) {
        auto& indexedFeature = std::get<1>(matchingBox);
        auto& sourceLayerName = indexedFeature.sourceLayerName;
        auto& bucketName = indexedFeature.bucketName;

        auto& layerResult = result[bucketName];

        layerResult.push_back(sourceLayerName);
    }

}
