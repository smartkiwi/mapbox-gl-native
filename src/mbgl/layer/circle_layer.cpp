#include <mbgl/layer/circle_layer.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/util/get_geometries.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> CircleLayer::clone() const {
    return std::make_unique<CircleLayer>(*this);
}

void CircleLayer::parsePaints(const JSValue& layer) {
    paint.radius.parse("circle-radius", layer);
    paint.color.parse("circle-color", layer);
    paint.opacity.parse("circle-opacity", layer);
    paint.translate.parse("circle-translate", layer);
    paint.translateAnchor.parse("circle-translate-anchor", layer);
    paint.blur.parse("circle-blur", layer);
}

void CircleLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.radius.cascade(parameters);
    paint.color.cascade(parameters);
    paint.opacity.cascade(parameters);
    paint.translate.cascade(parameters);
    paint.translateAnchor.cascade(parameters);
    paint.blur.cascade(parameters);
}

bool CircleLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.radius.calculate(parameters);
    hasTransitions |= paint.color.calculate(parameters);
    hasTransitions |= paint.opacity.calculate(parameters);
    hasTransitions |= paint.translate.calculate(parameters);
    hasTransitions |= paint.translateAnchor.calculate(parameters);
    hasTransitions |= paint.blur.calculate(parameters);

    passes = paint.isVisible() ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> CircleLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<CircleBucket>(parameters.mode);

    auto& name = bucketName();
    parameters.eachFilteredFeature(filter, [&] (const auto& feature, std::size_t index, const std::string& layerName) {
        auto geometries = getGeometries(feature);
        bucket->addGeometry(geometries);
        parameters.featureIndex.insert(geometries, index, layerName, name);
    });

    return std::move(bucket);
}

float CircleLayer::getQueryRadius() const {
    const std::array<float, 2>& translate = paint.translate;
    return paint.radius + util::length(translate[0], translate[1]);
}

bool CircleLayer::queryIntersectsGeometry(
        const GeometryCollection& queryGeometry,
        const GeometryCollection& geometry,
        const float bearing,
        const float pixelsToTileUnits) const {                

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry, paint.translate, paint.translateAnchor, bearing, pixelsToTileUnits);

    auto circleRadius = paint.radius * pixelsToTileUnits;

    return util::multiPolygonIntersectsBufferedMultiPoint(
            translatedQueryGeometry.value_or(queryGeometry), geometry, circleRadius);
}

} // namespace mbgl
