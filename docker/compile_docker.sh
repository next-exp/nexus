#!/bin/bash

source /nexus/docker/env.sh

cmake -DGSL_ROOT_DIR=/usr/include/gsl -DHDF5_ROOT=/opt/conda/envs/tests/ -DCMAKE_INSTALL_PREFIX=$NEXUSDIR -S . -B /nexus-build
cmake --build /nexus-build --target install -j`nproc`
