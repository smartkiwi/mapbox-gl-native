#ifndef MBGL_CIRCLE_LAYER
#define MBGL_CIRCLE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class CirclePaintProperties {
public:
    PaintProperty<float> radius { 5.0f };
    PaintProperty<Color> color { {{ 0, 0, 0, 1 }} };
    PaintProperty<float> opacity { 1.0f };
    PaintProperty<std::array<float, 2>> translate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> translateAnchor { TranslateAnchorType::Map };
    PaintProperty<float> blur { 0 };

    bool isVisible() const {
        return radius > 0 && color.value[3] > 0 && opacity > 0;
    }
};

class CircleLayer : public StyleLayer {
public:
    CircleLayer() : StyleLayer(Type::Circle) {}
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSValue&) override {};
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    float getQueryRadius() const override;
    bool queryIntersectsGeometry(
            const GeometryCollection& queryGeometry,
            const GeometryCollection& geometry,
            const float bearing,
            const float pixelsToTileUnits) const override;

    CirclePaintProperties paint;
};

template <>
inline bool StyleLayer::is<CircleLayer>() const {
    return type == Type::Circle;
}

} // namespace mbgl

#endif
