FROM registry.cern.ch/next/nexus-base:2024-02-20

COPY . /nexus
WORKDIR /nexus

RUN bash /nexus/docker/compile_docker.sh
