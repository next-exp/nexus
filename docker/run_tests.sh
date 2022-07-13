#!/bin/bash

source /nexus/docker/env.sh

source /opt/conda/etc/profile.d/conda.sh
conda activate tests

/nexus/bin/nexus-test && pytest -v
