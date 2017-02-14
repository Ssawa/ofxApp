#!/usr/bin/env bash

cd $OF_ROOT/addons

grep "^[^#;]" ./$(basename $TRAVIS_BUILD_DIR)/deps.txt | xargs -L 1 git clone --depth 1
