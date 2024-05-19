# latest, noble
# noble=24.04
ARG VERSION="latest"
FROM ubuntu:${VERSION}

# suppress interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qq && export DEBIAN_FRONTEND=noninteractive && \
    apt-get install -y --no-install-recommends software-properties-common \
    build-essential git openssh-client curl wget graphviz ninja-build \
    gcc g++ gdb clang lldb lld libclang-dev libclang-cpp-dev clangd \
    clang-tidy clang-format llvm-dev llvm-runtime libclang-rt-dev \
    cmake cmake-curses-gui make ccache autoconf automake cppcheck \
    doxygen valgrind \
    python3 python3-pip pipx \
    neovim fish \
    && pipx ensurepath \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Ensure the pipx binary path is available
ENV PATH=/root/.local/bin:$PATH \
    CC="clang" \
    CXX="clang++"

# Install conan
RUN pipx install conan &&  conan --version

# Entry point script to set umask and start fish shell
ENTRYPOINT ["sh", "-c", "umask 0022 && exec /bin/fish"]
