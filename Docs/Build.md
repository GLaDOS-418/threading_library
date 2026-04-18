# Build And Tooling

## Requirements

- Linux
- CMake
- Conan
- A C++20-capable compiler
- `g++` is the primary verified path for this repository
- `clang++` should also work for normal builds

## Default Build

1. Install dependencies:

   ```sh
   make deps
   ```

2. Configure the default debug tree:

   ```sh
   make configure
   ```

3. Build everything:

   ```sh
   make build
   ```

4. Run tests:

   ```sh
   make test
   ```

The default build directory is `_build/debug`.

## Direct CMake Examples

If you prefer to drive CMake yourself, install Conan dependencies first and then configure explicitly:

```sh
conan install . --output-folder=_build/debug --build=missing \
  --profile:build=./conan.profile --profile:host=./conan.profile

cmake -S . -B _build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=$(pwd)/_build/debug/conan_toolchain.cmake
```

Examples of enabling CMake options directly from the terminal:

```sh
cmake -S . -B _build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=$(pwd)/_build/debug/conan_toolchain.cmake \
  -DENABLE_IWYU=ON
```

```sh
cmake -S . -B _build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=$(pwd)/_build/debug/conan_toolchain.cmake \
  -DENABLE_CLANG_TIDY=ON \
  -DENABLE_HARDENING=ON
```

```sh
cmake -S . -B _build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=$(pwd)/_build/debug/conan_toolchain.cmake \
  -DENABLE_ASAN=ON
```

Sanitizers are probed at configure time. If the active compiler/runtime does not
support a requested sanitizer, configuration fails early with a direct message.

## Examples

Build only the example applications:

```sh
make examples
```

## IWYU

Run include analysis:

```sh
make iwyu
```

Run include analysis and auto-apply fixes:

```sh
make iwyu-fix
```

If you want IWYU rationale comments added to include lines:

```sh
make iwyu-fix IWYU_FIX_ARGS="--nosafe_headers --comments --update_comments"
```

## CMake Formatting

Format the repository's CMake files with the checked-in policy:

```sh
make format-cmake
```

This uses [.cmake-format.yaml](../.cmake-format.yaml).

## Doxygen

Generate HTML documentation:

```sh
make docs
```

Serve the generated documentation locally:

```sh
make docs-serve
```

The generated HTML is written under `_build/docs/html`.

## Dev Container

This repository includes a VS Code dev container configuration under `.devcontainer/`.

Conceptually, a dev container is just a reproducible development environment
described with a Dockerfile plus a small metadata file. Instead of installing
all compilers, tools, and editor integrations directly on the host machine, you
open the repository inside that container and work against the same toolchain
every time.

Typical usage in VS Code:

1. Install the `Dev Containers` extension.
2. Open the repository in VS Code.
3. Run `Dev Containers: Reopen in Container` from the command palette.
4. Once the container finishes building, run the normal repo commands from the integrated terminal:

   ```sh
   make deps
   make build
   make test
   ```

The container installs GCC, Clang, Conan, CMake, clang-tidy, Doxygen, and the
other tools used by this repository.

### Dev Containers And Neovim

The devcontainer metadata is designed primarily for VS Code and the Dev
Containers ecosystem. That is the smoothest path if you want one-click "open
this repo in the container" behavior.

You can still use the same container idea with Neovim, but usually not through
the `devcontainer.json` file directly. The typical approaches are:

1. Start the container with Docker or a devcontainer-compatible CLI.
2. Open a shell inside the running container.
3. Run `nvim` inside that shell, or attach your terminal to the container and
   work there.

So the short answer is:

- VS Code: direct first-class devcontainer support.
- Neovim: yes, but usually by entering the container shell and running Neovim
  inside it rather than by having Neovim interpret devcontainer metadata itself.
