FROM ubuntu:jammy

ENV ROOT_VERSION 6.26.04

# if x11 add: libx11-dev libxpm4 libxpm-dev libxft2 libxft-dev libxext6 libxext-dev
RUN apt-get update -y && \
    apt-get install -y cmake curl g++ libexpat1-dev git python3 zlib1g zlib1g-dev libssl-dev && \
    apt-get autoremove -y && \
    apt-get clean -y && \
    rm -rf /var/cache/apt/archives/* && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir -p /opt/root/src && \
    mkdir -p /opt/root/build && \
    mkdir -p /opt/root/install && \
    curl -o /root.tar.gz https://root.cern/download/root_v${ROOT_VERSION}.source.tar.gz && \
    tar xf /root.tar.gz -C /opt/root/src && \
    cd /opt/root/build && \
    cmake -Druntime_cxxmodules=OFF -Dxrootd=OFF -Dbuiltin_xrootd=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX=/opt/root/install -Dx11=OFF ../src/root-${ROOT_VERSION} && \
    make -j`nproc` && \
    make install && \
    rm -r /opt/root/src /opt/root/build /root.tar.gz
