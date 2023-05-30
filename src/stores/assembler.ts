import { defineStore } from 'pinia';
import { restoreStoreState, saveStoreState } from '.';

export const useAssemblerStore = defineStore('assembler', {
  state: () => ({
    showOutput: true,
    outputRadix: 16,
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
    },
  },
});
