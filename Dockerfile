FROM registry.cern.ch/next/nexus-base:2022-07-13

COPY . /nexus
WORKDIR /nexus

RUN bash /nexus/docker/compile_docker.sh
