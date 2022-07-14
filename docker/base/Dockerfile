FROM ubuntu:jammy

RUN apt-get update -y && \
    apt-get install -y cmake g++ libexpat1-dev libgsl27 libgsl-dev &&\ 
    apt-get autoremove -y && \
    apt-get clean -y && \
    rm -rf /var/cache/apt/archives/* && \
    rm -rf /var/lib/apt/lists/*

COPY --from=registry.cern.ch/next/geant4:11.0.2           /opt/geant4       /opt/geant4
COPY --from=registry.cern.ch/next/root:6.26.04            /opt/root/install /opt/root/install
COPY --from=registry.cern.ch/next/python-nexus:2022-07-13 /opt/conda        /opt/conda
