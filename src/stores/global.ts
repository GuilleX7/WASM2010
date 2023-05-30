import { getAsm2010Instance, loadAsm2010Instance } from '@/core/ts';
import { CsPlatform } from '@/core/ts/types';
import { defineStore } from 'pinia';
import { restoreStoreState, saveStoreState } from '.';
import { useAssemblerStore } from './assembler';
import { useEmulatorStore } from './emulator';

export const useGlobalStore = defineStore('global', {
  state: () => ({
    csPlatform: CsPlatform.Cs2010,
  }),
  actions: {
    async init(): Promise<void> {
      const savedState = restoreStoreState(this.$id);
      if (savedState) {
        this.$patch(savedState);
      } else {
        this.$reset();
      }

      this.$subscribe((_, state) => saveStoreState(this.$id, state));

      await useAssemblerStore().init();
      await useEmulatorStore().init();

      await loadAsm2010Instance(this.csPlatform);
    },
    switchCsPlatform(csPlatform: CsPlatform) {
      this.csPlatform = csPlatform;
      getAsm2010Instance().switchCsPlatform(csPlatform);
    },
  },
});
