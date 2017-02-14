# Tests the travis-ci build process with docker

FROM ubuntu:trusty

ARG build_dir=/build/ofxApp

ENV TRAVIS_BUILD_DIR $build_dir
ENV TARGET linux64
ENV OF_BRANCH master
ENV OF_ROOT /root/openFrameworks

RUN apt-get -y update
RUN apt-get -y install software-properties-common python-software-properties
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get -y update
RUN apt-get -y install gcc-4.9
RUN apt-get -y install g++-4.9
RUN apt-get -y install gdb
RUN apt-get -y install git
RUN apt-get -y install wget

RUN git clone --depth=1 --branch=$OF_BRANCH https://github.com/openframeworks/openFrameworks $OF_ROOT
RUN $OF_ROOT/scripts/ci/$TARGET/install.sh
RUN $OF_ROOT/scripts/dev/download_libs.sh -a 64;

RUN apt-get -y install nano

# Docker build restricts your build context so that you can't go outside your working folder.
# To get around this you should build with: docker build -t ofxapp -f $(REPO_PATH)/scripts/docker/Dockerfile $(REPO_PATH)
COPY . $build_dir

#RUN $OF_ROOT/scripts/ci/addons/install.sh
#RUN $OF_ROOT/addons/$(basename $TRAVIS_BUILD_DIR)/scripts/ci/build.sh
