#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/util/get_geometries.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> FillLayer::clone() const {
    return std::make_unique<FillLayer>(*this);
}

void FillLayer::parsePaints(const JSValue& layer) {
    paint.antialias.parse("fill-antialias", layer);
    paint.opacity.parse("fill-opacity", layer);
    paint.color.parse("fill-color", layer);
    paint.outlineColor.parse("fill-outline-color", layer);
    paint.translate.parse("fill-translate", layer);
    paint.translateAnchor.parse("fill-translate-anchor", layer);
    paint.pattern.parse("fill-pattern", layer);
}

void FillLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.antialias.cascade(parameters);
    paint.opacity.cascade(parameters);
    paint.color.cascade(parameters);
    paint.outlineColor.cascade(parameters);
    paint.translate.cascade(parameters);
    paint.translateAnchor.cascade(parameters);
    paint.pattern.cascade(parameters);
}

bool FillLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.antialias.calculate(parameters);
    hasTransitions |= paint.opacity.calculate(parameters);
    hasTransitions |= paint.color.calculate(parameters);
    hasTransitions |= paint.outlineColor.calculate(parameters);
    hasTransitions |= paint.translate.calculate(parameters);
    hasTransitions |= paint.translateAnchor.calculate(parameters);
    hasTransitions |= paint.pattern.calculate(parameters);

    passes = RenderPass::None;

    if (paint.antialias) {
        passes |= RenderPass::Translucent;
    }

    if (!paint.pattern.value.from.empty() || (paint.color.value[3] * paint.opacity) < 1.0f) {
        passes |= RenderPass::Translucent;
    } else {
        passes |= RenderPass::Opaque;
    }

    return hasTransitions;
}

std::unique_ptr<Bucket> FillLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<FillBucket>();

    auto& name = bucketName();
    parameters.eachFilteredFeature(filter, [&] (const auto& feature, std::size_t index, const std::string& layerName) {
        auto geometries = getGeometries(feature);
        bucket->addGeometry(geometries);
        parameters.featureIndex.insert(geometries, index, layerName, name);
    });

    return std::move(bucket);
}

float FillLayer::getQueryRadius() const {
    const std::array<float, 2>& translate = paint.translate;
    return util::length(translate[0], translate[1]);
}

bool FillLayer::queryIntersectsGeometry(
        const GeometryCollection& queryGeometry,
        const GeometryCollection& geometry,
        const float bearing,
        const float pixelsToTileUnits) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry, paint.translate, paint.translateAnchor, bearing, pixelsToTileUnits);

    return util::multiPolygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), geometry);
}

} // namespace mbgl
