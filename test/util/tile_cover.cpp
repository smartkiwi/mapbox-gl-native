#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/geo.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

TEST(TileCover, Empty) {
    EXPECT_EQ((std::set<UnwrappedTileID>{}), tileCover(LatLngBounds::empty(), 0));
}

TEST(TileCover, Arctic) {
    EXPECT_EQ((std::set<UnwrappedTileID>{}),
              tileCover(LatLngBounds::hull({ 86, -180 }, { 90, 180 }), 0));
}

TEST(TileCover, Antarctic) {
    EXPECT_EQ((std::set<UnwrappedTileID>{}),
              tileCover(LatLngBounds::hull({ -86, -180 }, { -90, 180 }), 0));
}

TEST(TileCover, WorldZ0) {
    EXPECT_EQ((std::set<UnwrappedTileID>{
                  { 0, 0, 0 },
              }),
              tileCover(LatLngBounds::world(), 0));
}

TEST(TileCover, WorldZ1) {
    EXPECT_EQ((std::set<UnwrappedTileID>{
                  { 1, 1, 1 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 0, 0 },
              }),
              tileCover(LatLngBounds::world(), 1));
}

TEST(TileCover, SingletonZ0) {
    EXPECT_EQ((std::set<UnwrappedTileID>{}), tileCover(LatLngBounds::singleton({ 0, 0 }), 0));
}

TEST(TileCover, SingletonZ1) {
    EXPECT_EQ((std::set<UnwrappedTileID>{}), tileCover(LatLngBounds::singleton({ 0, 0 }), 1));
}

static const LatLngBounds sanFrancisco =
    LatLngBounds::hull({ 37.6609, -122.5744 }, { 37.8271, -122.3204 });

TEST(TileCover, SanFranciscoZ0) {
    EXPECT_EQ((std::set<UnwrappedTileID>{
                  { 0, 0, 0 },
              }),
              tileCover(sanFrancisco, 0));
}

TEST(TileCover, SanFranciscoZ10) {
    EXPECT_EQ((std::set<UnwrappedTileID>{
                  { 10, 163, 395 }, { 10, 164, 395 }, { 10, 163, 396 }, { 10, 164, 396 },

              }),
              tileCover(sanFrancisco, 10));
}

static const LatLngBounds sanFranciscoWrapped =
    LatLngBounds::hull({ 37.6609, 238.5744 }, { 37.8271, 238.3204 });

TEST(TileCover, SanFranciscoZ0Wrapped) {
    EXPECT_EQ((std::set<UnwrappedTileID>{ { 0, 1, 0 } }), tileCover(sanFranciscoWrapped, 0));
}
