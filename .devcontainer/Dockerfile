ARG VERSION="25.10"
FROM ubuntu:${VERSION}

ENV DEBIAN_FRONTEND=noninteractive
ENV PATH=/root/.local/bin:$PATH

RUN apt-get update -qq && \
    apt-get install -y --no-install-recommends \
        autoconf \
        automake \
        build-essential \
        ca-certificates \
        ccache \
        clang \
        clangd \
        clang-format \
        clang-tidy \
        cmake \
        cmake-curses-gui \
        cppcheck \
        curl \
        doxygen \
        fish \
        g++ \
        gcc \
        gdb \
        git \
        graphviz \
        libclang-cpp-dev \
        libclang-dev \
        libclang-rt-dev \
        lldb \
        lld \
        llvm-dev \
        llvm-runtime \
        make \
        neovim \
        ninja-build \
        openssh-client \
        pipx \
        python3 \
        python3-pip \
        python3-venv \
        software-properties-common \
        valgrind \
        wget && \
    pipx install conan && \
    pipx install cmakelang && \
    conan --version && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
