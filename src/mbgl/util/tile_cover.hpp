#ifndef MBGL_UTIL_TILE_COVER
#define MBGL_UTIL_TILE_COVER

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/style/types.hpp>

#include <set>

namespace mbgl {

class TransformState;
class LatLngBounds;

int32_t coveringZoomLevel(double z, SourceType type, uint16_t tileSize);

std::set<UnwrappedTileID> tileCover(const TransformState&, int32_t z);
std::set<UnwrappedTileID> tileCover(const LatLngBounds&, int32_t z);

} // namespace mbgl

#endif
