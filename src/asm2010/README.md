# ASM2010

This folder contains the source for building the ASM2010 binary for the wasi32-wasm target platform, together with some Typescript glue code to interface with the binary from the browser.

## Building

While I've already provided you with a prebuilt binary, you might want to rebuild it on your own to make further customizations.

If that's the case, you will need:

- LLVM/Clang with support for the WebAssembly target (any version from 9.0 onwards). Remember you will need the builtins library for wasm32
- The standard C library for WebAssembly/WASI
- Make-like tool

It shouldn't be too difficult to download them from their repositories. However, the best choice by far is to download the latest release of the WASI SDK, which is available at https://github.com/WebAssembly/wasi-sdk. It is a ready-to-go installation of LLVM/Clang with everything mentioned above already installed, making it really useful to build WebAssembly/WASI modules.

Then, open the Makefile and change the "WASI_SDK" variable so it points to your local WASI SDK folder:

```make
# Your WASI SDK path
WASI_SDK=C:\Users\GuilleX7\Downloads\wasi-sdk-16.0 (in Windows)
WASI_SDK=/home/guillex7/Downloads/wasi-sdk-16.0 (in Linux)

# You probably don't need to change anything here
CC=$(WASI_SDK)/bin/clang
WASI_SYSROOT=$(WASI_SDK)/share/wasi-sysroot

...
```

In the case that you already have a compliant LLVM installation or you don't want to use the WASI SDK, you can change CC and WASI_SYSROOT so they point to your Clang binary and your WASI sysroot, respectively.

Open a console and type:

```
make
```

This will build a binary called `asm2010.wasm` in the root directory. To clean up the object files, use:

```
make clean
```
