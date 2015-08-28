#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_mason" \
git submodule update --init .mason

export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

if [ ${TRAVIS_OS_NAME} == "linux" ]; then
    mapbox_time "install_mesa" \
    mason install mesa 10.4.3
fi

mapbox_time "install_nvm" \
curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.26.1/install.sh | bash

mapbox_time $NODE_VERSION \
nvm install $NODE_VERSION

node --version
npm --version