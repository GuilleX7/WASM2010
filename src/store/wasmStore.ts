import { asmAssemble, loadAsm2010, TAsParseResult } from '@/wasm/asm2010';
import { defineStore } from 'pinia';

const useWasmStore = defineStore('wasm', {
  state: (): {
    isLoaded: boolean;
  } => ({
    isLoaded: false,
  }),
  actions: {
    async load() {
      await loadAsm2010();
      this.isLoaded = true;
    },
    assemble(sourceAssembly: string): TAsParseResult {
      return asmAssemble(sourceAssembly);
    },
  },
});

export { useWasmStore };
