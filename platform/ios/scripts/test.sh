#!/usr/bin/env bash

set -e
set -o pipefail
set -u

mkdir -p build/ios-all/gyp/ios.xcodeproj/xcshareddata/xcschemes
cp platform/ios/scripts/test.xcscheme build/ios-all/gyp/ios.xcodeproj/xcshareddata/xcschemes/test.xcscheme

xcodebuild \
    -project ./build/ios-all/gyp/ios.xcodeproj \
    -scheme 'test' \
    -sdk iphonesimulator \
    -destination 'platform=iOS Simulator,name=iPhone 6,OS=latest' \
    -derivedDataPath ./build/ios/test \
    build | xcpretty

ios-sim start
ios-sim launch ./build/ios/test/Build/Products/Debug-iphonesimulator/ios-test.app
