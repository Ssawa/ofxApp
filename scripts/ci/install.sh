#!/usr/bin/env bash

# Downloads all of our dependent addons

cd $OF_ROOT/addons

grep "^[^#;]" $TRAVIS_BUILD_DIR/deps.txt | xargs -L 1 git clone --depth 1

exit 0