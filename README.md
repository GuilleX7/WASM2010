# WASM2010
WebAssembly port of ASM2010:

- CS2010 architecture: https://www.dte.us.es/docencia/etsii/gii-is/estructura-de-computadores/grupo-4-2018/CS2010-2013.pdf
- ASM2010 repository: https://github.com/GuilleX7/ASM2010

## Development and building

First, install the required dependencies:

```
npm ci
```

Then, you can start a development server:

```
npm run dev
```

Or you can build the toolkit for production:

```
npm run build
```

Please take into account that the project requires an ASM2010 binary to be placed in 'src/asm2010/build'. A prebuilt binary is already featured in this project, but you can find instructions on how to build it in the 'src/asm2010' folder.
